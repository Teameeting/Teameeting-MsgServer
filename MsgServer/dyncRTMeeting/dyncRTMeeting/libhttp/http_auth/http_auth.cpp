#include "http_auth.h"
#include <assert.h>
#include <time.h>
#include <sstream>

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define SECURITY_WIN32
#include <security.h>
#endif
#include "base64.h"
#include "basictypes.h"
#include "messagedigest.h"


template<class CTYPE>
size_t sprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format, ...);

///////////////////////////////////////////////////////////////////////////////
// Generic string/memory utilities
///////////////////////////////////////////////////////////////////////////////

#define STACK_ARRAY(TYPE, LEN) static_cast<TYPE*>(::alloca((LEN)*sizeof(TYPE)))

size_t escape(char * buffer, size_t buflen,
              const char * source, size_t srclen,
              const char * illegal, char escape) {
  if (buffer == NULL || buflen <= 0)
    return 0;

  size_t srcpos = 0, bufpos = 0;
  while ((srcpos < srclen) && (bufpos + 1 < buflen)) {
    char ch = source[srcpos++];
    if ((ch == escape) || ::strchr(illegal, ch)) {
      if (bufpos + 2 >= buflen)
        break;
      buffer[bufpos++] = escape;
    }
    buffer[bufpos++] = ch;
  }

  buffer[bufpos] = '\0';
  return bufpos;
}

size_t unescape(char * buffer, size_t buflen,
                const char * source, size_t srclen,
                char escape) {
  if (buffer == NULL || buflen <= 0)
    return 0;

  size_t srcpos = 0, bufpos = 0;
  while ((srcpos < srclen) && (bufpos + 1 < buflen)) {
    char ch = source[srcpos++];
    if ((ch == escape) && (srcpos < srclen)) {
      ch = source[srcpos++];
    }
    buffer[bufpos++] = ch;
  }
  buffer[bufpos] = '\0';
  return bufpos;
}

template<class CTYPE>
size_t strlenn(const CTYPE* buffer, size_t buflen) {
  size_t bufpos = 0;
  while (buffer[bufpos] && (bufpos < buflen)) {
    ++bufpos;
  }
  return bufpos;
}
// Safe versions of strncpy, strncat, snprintf and vsnprintf that always
// null-terminate.
const size_t SIZE_UNKNOWN = static_cast<size_t>(-1);
template<class CTYPE>
size_t strcpyn(CTYPE* buffer, size_t buflen,
               const CTYPE* source, size_t srclen = SIZE_UNKNOWN) {
  if (buflen <= 0)
    return 0;

  if (srclen == SIZE_UNKNOWN) {
    srclen = strlenn(source, buflen - 1);
  } else if (srclen >= buflen) {
    srclen = buflen - 1;
  }
  memcpy(buffer, source, srclen * sizeof(CTYPE));
  buffer[srclen] = 0;
  return srclen;
}

inline bool IsEndOfAttributeName(size_t pos, size_t len, const char * data) {
  if (pos >= len)
    return true;
  if (isspace(static_cast<unsigned char>(data[pos])))
    return true;
  // The reason for this complexity is that some attributes may contain trailing
  // equal signs (like base64 tokens in Negotiate auth headers)
  if ((pos+1 < len) && (data[pos] == '=') &&
      !isspace(static_cast<unsigned char>(data[pos+1])) &&
      (data[pos+1] != '=')) {
    return true;
  }
  return false;
}

// TODO: unittest for EscapeAttribute and HttpComposeAttributes.

std::string EscapeAttribute(const std::string& attribute) {
  const size_t kMaxLength = attribute.length() * 2 + 1;
  char* buffer = STACK_ARRAY(char, kMaxLength);
  size_t len = escape(buffer, kMaxLength, attribute.data(), attribute.length(),
                      "\"", '\\');
  return std::string(buffer, len);
}

void HttpComposeAttributes(const HttpAttributeList& attributes, char separator,
                           std::string* composed) {
  std::stringstream ss;
  for (size_t i=0; i<attributes.size(); ++i) {
    if (i > 0) {
      ss << separator << " ";
    }
    ss << attributes[i].first;
    if (!attributes[i].second.empty()) {
      ss << "=\"" << EscapeAttribute(attributes[i].second) << "\"";
    }
  }
  *composed = ss.str();
}

void HttpParseAttributes(const char * data, size_t len,
                         HttpAttributeList& attributes) {
  size_t pos = 0;
  while (true) {
    // Skip leading whitespace
    while ((pos < len) && isspace(static_cast<unsigned char>(data[pos]))) {
      ++pos;
    }

    // End of attributes?
    if (pos >= len)
      return;

    // Find end of attribute name
    size_t start = pos;
    while (!IsEndOfAttributeName(pos, len, data)) {
      ++pos;
    }

    HttpAttribute attribute;
    attribute.first.assign(data + start, data + pos);

    // Attribute has value?
    if ((pos < len) && (data[pos] == '=')) {
      ++pos; // Skip '='
      // Check if quoted value
      if ((pos < len) && (data[pos] == '"')) {
        while (++pos < len) {
          if (data[pos] == '"') {
            ++pos;
            break;
          }
          if ((data[pos] == '\\') && (pos + 1 < len))
            ++pos;
          attribute.second.append(1, data[pos]);
        }
      } else {
        while ((pos < len) &&
            !isspace(static_cast<unsigned char>(data[pos])) &&
            (data[pos] != ',')) {
          attribute.second.append(1, data[pos++]);
        }
      }
    }

    attributes.push_back(attribute);
    if ((pos < len) && (data[pos] == ',')) ++pos; // Skip ','
  }
}

bool HttpHasAttribute(const HttpAttributeList& attributes,
                      const std::string& name,
                      std::string* value) {
  for (HttpAttributeList::const_iterator it = attributes.begin();
       it != attributes.end(); ++it) {
    if (it->first == name) {
      if (value) {
        *value = it->second;
      }
      return true;
    }
  }
  return false;
}

bool HttpHasNthAttribute(HttpAttributeList& attributes,
                         size_t index,
                         std::string* name,
                         std::string* value) {
  if (index >= attributes.size())
    return false;

  if (name)
    *name = attributes[index].first;
  if (value)
    *value = attributes[index].second;
  return true;
}


std::string quote(const std::string& str) {
  std::string result;
  result.push_back('"');
  for (size_t i=0; i<str.size(); ++i) {
    if ((str[i] == '"') || (str[i] == '\\'))
      result.push_back('\\');
    result.push_back(str[i]);
  }
  result.push_back('"');
  return result;
}

#if defined(WIN32)
struct NegotiateAuthContext : public HttpAuthContext {
  CredHandle cred;
  CtxtHandle ctx;
  size_t steps;
  bool specified_credentials;

  NegotiateAuthContext(const std::string& auth, CredHandle c1, CtxtHandle c2)
  : HttpAuthContext(auth), cred(c1), ctx(c2), steps(0),
    specified_credentials(false)
  { }

  virtual ~NegotiateAuthContext() {
    DeleteSecurityContext(&ctx);
    FreeCredentialsHandle(&cred);
  }
};
#endif // WIN32

HttpAuthResult HttpAuthenticate(
  const char * challenge, size_t len,
  const std::string& server,
  const std::string& method, const std::string& uri,
  const std::string& username, const std::string& password,
  HttpAuthContext *& context, std::string& response, std::string& auth_method)
{
#if TEST_DIGEST
  challenge = DIGEST_CHALLENGE;
  len = strlen(challenge);
#endif

  HttpAttributeList args;
  HttpParseAttributes(challenge, len, args);
  HttpHasNthAttribute(args, 0, &auth_method, NULL);

  if (context && (context->auth_method != auth_method))
    return HAR_IGNORE;

  // BASIC
  if (_stricmp(auth_method.c_str(), "basic") == 0) {
    if (context)
      return HAR_CREDENTIALS; // Bad credentials
    if (username.empty())
      return HAR_CREDENTIALS; // Missing credentials

    context = new HttpAuthContext(auth_method);

    // TODO: convert sensitive to a secure buffer that gets securely deleted
    //std::string decoded = username + ":" + password;
	size_t len = username.size() + password.size() + 2;
    char * sensitive = new char[len];
    size_t pos = strcpyn(sensitive, len, username.data(), username.size());
    pos += strcpyn(sensitive + pos, len - pos, ":");
	memcpy(sensitive + pos, password.c_str(), password.size());

    response = auth_method;
    response.append(" ");
    // TODO: create a sensitive-source version of Base64::encode
	response.append(rtc::Base64::Encode(sensitive));
    memset(sensitive, 0, len);
    delete [] sensitive;
    return HAR_RESPONSE;
  }

  // DIGEST
  if (_stricmp(auth_method.c_str(), "digest") == 0) {
    if (context)
      return HAR_CREDENTIALS; // Bad credentials
    if (username.empty())
      return HAR_CREDENTIALS; // Missing credentials

    context = new HttpAuthContext(auth_method);

    std::string cnonce, ncount;
#if TEST_DIGEST
    method = DIGEST_METHOD;
    uri    = DIGEST_URI;
    cnonce = DIGEST_CNONCE;
#else
    char buffer[256];
    sprintf(buffer, "%d", static_cast<int>(time(0)));
	cnonce = rtc::MD5(buffer);
#endif
    ncount = "00000001";

    std::string realm, nonce, qop, opaque;
    HttpHasAttribute(args, "realm", &realm);
    HttpHasAttribute(args, "nonce", &nonce);
    bool has_qop = HttpHasAttribute(args, "qop", &qop);
    bool has_opaque = HttpHasAttribute(args, "opaque", &opaque);

    // TODO: convert sensitive to be secure buffer
    //std::string A1 = username + ":" + realm + ":" + password;
	size_t len = username.size() + realm.size() + password.size() + 3;
    char * sensitive = new char[len];  // A1
    size_t pos = strcpyn(sensitive, len, username.data(), username.size());
    pos += strcpyn(sensitive + pos, len - pos, ":");
    pos += strcpyn(sensitive + pos, len - pos, realm.c_str());
    pos += strcpyn(sensitive + pos, len - pos, ":");
   memcpy(sensitive + pos, password.c_str(), password.size());

    std::string A2 = method + ":" + uri;
    std::string middle;
    if (has_qop) {
      qop = "auth";
      middle = nonce + ":" + ncount + ":" + cnonce + ":" + qop;
    } else {
      middle = nonce;
    }
	std::string HA1 = rtc::MD5(sensitive);
    memset(sensitive, 0, len);
    delete [] sensitive;
    std::string HA2 = rtc::MD5(A2);
    std::string dig_response = rtc::MD5(HA1 + ":" + middle + ":" + HA2);

#if TEST_DIGEST
    ASSERT(strcmp(dig_response.c_str(), DIGEST_RESPONSE) == 0);
#endif

    std::stringstream ss;
    ss << auth_method;
    ss << " username=" << quote(username);
    ss << ", realm=" << quote(realm);
    ss << ", nonce=" << quote(nonce);
    ss << ", uri=" << quote(uri);
    if (has_qop) {
      ss << ", qop=" << qop;
      ss << ", nc="  << ncount;
      ss << ", cnonce=" << quote(cnonce);
    }
    ss << ", response=\"" << dig_response << "\"";
    if (has_opaque) {
      ss << ", opaque=" << quote(opaque);
    }
    response = ss.str();
    return HAR_RESPONSE;
  }

#if defined(WIN32)
#if 1
  bool want_negotiate = (_stricmp(auth_method.c_str(), "negotiate") == 0);
  bool want_ntlm = (_stricmp(auth_method.c_str(), "ntlm") == 0);
  // SPNEGO & NTLM
  if (want_negotiate || want_ntlm) {
    const size_t MAX_MESSAGE = 12000, MAX_SPN = 256;
    char out_buf[MAX_MESSAGE], spn[MAX_SPN];

#if 0 // Requires funky windows versions
    DWORD len = MAX_SPN;
    if (DsMakeSpn("HTTP", server.HostAsURIString().c_str(), NULL,
                  server.port(),
                  0, &len, spn) != ERROR_SUCCESS) {
      LOG_F(WARNING) << "(Negotiate) - DsMakeSpn failed";
      return HAR_IGNORE;
    }
#else
    sprintfn(spn, MAX_SPN, "HTTP/%s", server.c_str());
#endif

    SecBuffer out_sec;
    out_sec.pvBuffer   = out_buf;
    out_sec.cbBuffer   = sizeof(out_buf);
    out_sec.BufferType = SECBUFFER_TOKEN;

    SecBufferDesc out_buf_desc;
    out_buf_desc.ulVersion = 0;
    out_buf_desc.cBuffers  = 1;
    out_buf_desc.pBuffers  = &out_sec;

    const ULONG NEG_FLAGS_DEFAULT =
      //ISC_REQ_ALLOCATE_MEMORY
      ISC_REQ_CONFIDENTIALITY
      //| ISC_REQ_EXTENDED_ERROR
      //| ISC_REQ_INTEGRITY
      | ISC_REQ_REPLAY_DETECT
      | ISC_REQ_SEQUENCE_DETECT
      //| ISC_REQ_STREAM
      //| ISC_REQ_USE_SUPPLIED_CREDS
      ;

    ::TimeStamp lifetime;
    SECURITY_STATUS ret = S_OK;
    ULONG ret_flags = 0, flags = NEG_FLAGS_DEFAULT;

    bool specify_credentials = !username.empty();
    size_t steps = 0;

    //uint32 now = Time();

    NegotiateAuthContext * neg = static_cast<NegotiateAuthContext *>(context);
    if (neg) {
      const size_t max_steps = 10;
      if (++neg->steps >= max_steps) {
        printf("AsyncHttpsProxySocket::Authenticate(Negotiate) too many retries \r\n");
        return HAR_ERROR;
      }
      steps = neg->steps;

      std::string challenge, decoded_challenge;
      if (HttpHasNthAttribute(args, 1, &challenge, NULL)
          && rtc::Base64::Decode(challenge, rtc::Base64::DO_STRICT,
                            &decoded_challenge, NULL)) {
        SecBuffer in_sec;
        in_sec.pvBuffer   = const_cast<char *>(decoded_challenge.data());
        in_sec.cbBuffer   = static_cast<unsigned long>(decoded_challenge.size());
        in_sec.BufferType = SECBUFFER_TOKEN;

        SecBufferDesc in_buf_desc;
        in_buf_desc.ulVersion = 0;
        in_buf_desc.cBuffers  = 1;
        in_buf_desc.pBuffers  = &in_sec;

        ret = InitializeSecurityContextA(&neg->cred, &neg->ctx, spn, flags, 0, SECURITY_NATIVE_DREP, &in_buf_desc, 0, &neg->ctx, &out_buf_desc, &ret_flags, &lifetime);
        if (FAILED(ret)) {
          //LOG(LS_ERROR) << "InitializeSecurityContext returned: "<< ErrorName(ret, SECURITY_ERRORS);
          return HAR_ERROR;
        }
      } else if (neg->specified_credentials) {
        // Try again with default credentials
        specify_credentials = false;
        delete context;
        context = neg = 0;
      } else {
        return HAR_CREDENTIALS;
      }
    }

    if (!neg) {
      unsigned char userbuf[256], passbuf[256], domainbuf[16];
      SEC_WINNT_AUTH_IDENTITY_A auth_id, * pauth_id = 0;
      if (specify_credentials) {
        memset(&auth_id, 0, sizeof(auth_id));
        size_t len = password.size()+1;
        char * sensitive = new char[len];
		memcpy(sensitive, password.c_str(), password.size());
        std::string::size_type pos = username.find('\\');
        if (pos == std::string::npos) {
          auth_id.UserLength = static_cast<unsigned long>(
            rtc::_min(sizeof(userbuf) - 1, username.size()));
          memcpy(userbuf, username.c_str(), auth_id.UserLength);
          userbuf[auth_id.UserLength] = 0;
          auth_id.DomainLength = 0;
          domainbuf[auth_id.DomainLength] = 0;
          auth_id.PasswordLength = static_cast<unsigned long>(
			  rtc::_min(sizeof(passbuf) - 1, password.size()));
          memcpy(passbuf, sensitive, auth_id.PasswordLength);
          passbuf[auth_id.PasswordLength] = 0;
        } else {
          auth_id.UserLength = static_cast<unsigned long>(
            rtc::_min(sizeof(userbuf) - 1, username.size() - pos - 1));
          memcpy(userbuf, username.c_str() + pos + 1, auth_id.UserLength);
          userbuf[auth_id.UserLength] = 0;
          auth_id.DomainLength = static_cast<unsigned long>(
            rtc::_min(sizeof(domainbuf) - 1, pos));
          memcpy(domainbuf, username.c_str(), auth_id.DomainLength);
          domainbuf[auth_id.DomainLength] = 0;
          auth_id.PasswordLength = static_cast<unsigned long>(
            rtc::_min(sizeof(passbuf) - 1, password.size()));
          memcpy(passbuf, sensitive, auth_id.PasswordLength);
          passbuf[auth_id.PasswordLength] = 0;
        }
        memset(sensitive, 0, len);
        delete [] sensitive;
        auth_id.User = userbuf;
        auth_id.Domain = domainbuf;
        auth_id.Password = passbuf;
        auth_id.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
        pauth_id = &auth_id;
        printf("Negotiate protocol: Using specified credentials \r\n");
      } else {
        printf("Negotiate protocol: Using default credentials \r\n");
      }

      CredHandle cred;
      ret = AcquireCredentialsHandleA(
          0, const_cast<char*>(want_negotiate ? NEGOSSP_NAME_A : NTLMSP_NAME_A),
          SECPKG_CRED_OUTBOUND, 0, pauth_id, 0, 0, &cred, &lifetime);
      //LOG(INFO) << "$$$ AcquireCredentialsHandle @ " << TimeSince(now);
      if (ret != SEC_E_OK) {
        //LOG(LS_ERROR) << "AcquireCredentialsHandle error: "<< ErrorName(ret, SECURITY_ERRORS);
        return HAR_IGNORE;
      }

      //CSecBufferBundle<5, CSecBufferBase::FreeSSPI> sb_out;

      CtxtHandle ctx;
      ret = InitializeSecurityContextA(&cred, 0, spn, flags, 0, SECURITY_NATIVE_DREP, 0, 0, &ctx, &out_buf_desc, &ret_flags, &lifetime);
      //LOG(INFO) << "$$$ InitializeSecurityContext @ " << TimeSince(now);
      if (FAILED(ret)) {
        //LOG(LS_ERROR) << "InitializeSecurityContext returned: " << ErrorName(ret, SECURITY_ERRORS);
        FreeCredentialsHandle(&cred);
        return HAR_IGNORE;
      }

      assert(!context);
      context = neg = new NegotiateAuthContext(auth_method, cred, ctx);
      neg->specified_credentials = specify_credentials;
      neg->steps = steps;
    }

    if ((ret == SEC_I_COMPLETE_NEEDED) || (ret == SEC_I_COMPLETE_AND_CONTINUE)) {
      ret = CompleteAuthToken(&neg->ctx, &out_buf_desc);
      //LOG(INFO) << "$$$ CompleteAuthToken @ " << TimeSince(now);
      //LOG(LS_VERBOSE) << "CompleteAuthToken returned: " << ErrorName(ret, SECURITY_ERRORS);
      if (FAILED(ret)) {
        return HAR_ERROR;
      }
    }

    //LOG(INFO) << "$$$ NEGOTIATE took " << TimeSince(now) << "ms";

    std::string decoded(out_buf, out_buf + out_sec.cbBuffer);
    response = auth_method;
    response.append(" ");
    response.append(rtc::Base64::Encode(decoded));
    return HAR_RESPONSE;
  }
#endif
#endif // WIN32

  return HAR_IGNORE;
}