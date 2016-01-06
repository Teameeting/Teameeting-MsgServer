//
//  RTConnTcp.cpp
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTConnTcp.h"
#include "Assert.h"
#include "rtklog.h"
#include "RTMsgCommon.h"

RTConnTcp::RTConnTcp()
{

}

RTConnTcp::~RTConnTcp()
{

}

int RTConnTcp::DoProcessData(const char* pData, int nLen)
{
    if (nLen==0) {
        return 0;
    }
    memset((void*)&m_smsg, 0, sizeof(SIGNALMSG));
    memset((void*)&m_mmsg, 0, sizeof(MEETMSG));
    std::string sstr(pData, nLen), err;
    m_smsg.GetMsg(sstr, err);
    if (err.length() > 0) {
        LE("%s err:%s\n", __FUNCTION__, err.c_str());
        if (err.compare(INVALID_JSON_PARAMS)==0) {
            // Here means the msg not received totally
            return -1;
        } else {
            // Here means the msg error
            return nLen;
        }
    }
    if (m_smsg._scont.length()>0) {
        m_mmsg.GetMsg(m_smsg._scont, err);
        if (err.length() > 0) {
            LE("%s err:%s\n", __FUNCTION__, err.c_str());
            if (err.compare(INVALID_JSON_PARAMS)==0) {
                // Here means the msg not received totally
                return -1;
            } else {
                // Here means the msg error
                return nLen;
            }
        }
    }
    if (m_smsg._stype == SIGNALTYPE::reqkeepalive) {
        if (m_mmsg._from.length()>0) {
            OnKeepAlive(m_mmsg._from.c_str());
        } else {
            LE("keepalive params errors\n");
        }
    } else if (m_smsg._stype == SIGNALTYPE::reqlogin) {
        if (m_mmsg._from.length()>0 && m_mmsg._pass.length()>0) {
            OnLogin(m_mmsg._from.c_str(), m_mmsg._pass.c_str());
        } else {
            LE("login params errors\n");
        }
    } else if (m_smsg._stype == SIGNALTYPE::reqsndmsg) {
        if (m_mmsg._from.length()>0) {
            const char* pContentDump = strdup(m_mmsg.ToJson().c_str());
            OnSndMsg(m_mmsg._mtype, m_mmsg._mseq, m_mmsg._from.c_str(), pContentDump, (int)strlen(pContentDump));
            free((void*)pContentDump);
            pContentDump = NULL;
        } else {
            LE("sndmsg params errors\n");
        }
    } else if (m_smsg._stype == SIGNALTYPE::reqgetmsg) {
        if (m_mmsg._from.length()>0) {
            OnGetMsg(m_mmsg._mtype, m_mmsg._mseq, m_mmsg._from.c_str());
        } else {
            LE("getmsg params errors\n");
        }
    } else if (m_smsg._stype == SIGNALTYPE::reqlogout) {
        if (m_mmsg._from.length()>0) {
            OnLogout(m_mmsg._from.c_str());
        } else {
            LE("logout params errors\n");
        }
    } else {
        LE("parse signal msg params error\n");
    }
    return nLen;
}

int RTConnTcp::ProcessData(const char* pData, int nLen)
{
    int parsed = 0;
    int ll = 0;
    LI("RTConnTcp::ProcessData pData:%s\n", pData);
    while (parsed < nLen)
    {
        const char* pMsg = pData + parsed;
        int offset = 0;
        if (*(pMsg+offset) == '$') {
            offset += 1;
            char l[4] = {0};
            memcpy(l, pMsg+offset, 4);
            offset += 4;
            ll = (int)strtol(l, NULL, 10);
            if (ll>=0 && ll <= nLen) { // the message length may be 0
                int nlen = DoProcessData((char *)(pMsg+offset), ll);
                LI("DoProcessData msg nlen:%d, ll:%d\n", nlen, ll);
                if (nlen < 0) {
                    break;
                } else { // nlen < 0
                    assert(nlen == ll);
                    offset += ll;
                    parsed += offset;
                }
            } else { // ll>0 && ll <= nLen
                LE("Get Msg Len Error!!!\n");
            }
        }
    }
    return parsed;
}


char* RTConnTcp::GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen)
{
    return (char*)"";
}


void RTConnTcp::SendResponse(int code, const std::string&query, const char*pData, int nLen)
{

}

void RTConnTcp::SendResponse(int code, const std::string&strContent)
{
    OnResponse(strContent.c_str(), (int)strContent.length());
}