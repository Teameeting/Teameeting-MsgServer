#ifndef _http_auth_h_
#define _http_auth_h_
#include <string>
#include <vector>

typedef std::pair<std::string, std::string> HttpAttribute;
typedef std::vector<HttpAttribute> HttpAttributeList;
void HttpComposeAttributes(const HttpAttributeList& attributes, char separator,
                           std::string* composed);
void HttpParseAttributes(const char * data, size_t len,
                         HttpAttributeList& attributes);
bool HttpHasAttribute(const HttpAttributeList& attributes,
                      const std::string& name,
                      std::string* value);
bool HttpHasNthAttribute(HttpAttributeList& attributes,
                         size_t index,
                         std::string* name,
                         std::string* value);

//////////////////////////////////////////////////////////////////////
// Http Authentication
//////////////////////////////////////////////////////////////////////

struct HttpAuthContext {
  std::string auth_method;
  HttpAuthContext(const std::string& auth) : auth_method(auth) { }
  virtual ~HttpAuthContext() { }
};

enum HttpAuthResult { HAR_RESPONSE, HAR_IGNORE, HAR_CREDENTIALS, HAR_ERROR };

#ifdef __cplusplus
extern "C" {
#endif

// 'context' is used by this function to record information between calls.
// Start by passing a null pointer, then pass the same pointer each additional
// call.  When the authentication attempt is finished, delete the context.
HttpAuthResult HttpAuthenticate(
  const char * challenge, size_t len,
  const std::string& server,
  const std::string& method, const std::string& uri,
  const std::string& username, const std::string& password,
  HttpAuthContext *& context, std::string& response, std::string& auth_method);

#ifdef __cplusplus
}
#endif

#endif	// _http_auth_h_