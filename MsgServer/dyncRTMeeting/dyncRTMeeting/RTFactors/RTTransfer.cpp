//
//  RTTransfer.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <sstream>
#include "RTTransfer.h"
#include "http_common.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "RTMsgCommon.h"

RTTransfer::RTTransfer()
{

}

RTTransfer::~RTTransfer()
{

}

int RTTransfer::DoProcessData(const char *pData, int nLen)
{
    if (nLen==0) {
        return 0;
    }
    memset((void*)&m_msg, 0, sizeof(TRANSFERMSG));
    std::string str(pData, nLen), err;
    m_msg.GetMsg(str, err);
    if (err.length() > 0) {
        LE("%s TRANSFERMSG err::%s\n", err.c_str());
        if (err.compare(INVALID_JSON_PARAMS)==0) {
            // Here means the msg not received totally
            return -1;
        } else {
            // Here means the msg error
            return nLen;
        }
    }
    {
        // handle ack
        if (m_msg._action == TRANSFERACTION::req) {
            TRANSFERMSG ack_msg;
            ack_msg._action = TRANSFERACTION::req_ack;
            ack_msg._fmodule = TRANSFERMODULE::mmeeting;
            ack_msg._type   = m_msg._type;
            ack_msg._trans_seq = m_msg._trans_seq;
            ack_msg._trans_seq_ack = m_msg._trans_seq + 1;
            ack_msg._valid = m_msg._valid;
            ack_msg._content = "";
            const std::string s = ack_msg.ToJson();
            OnTransfer(s);
        } else if (m_msg._action == TRANSFERACTION::req_ack) {
            if (m_msg._trans_seq + 1 == m_msg._trans_seq_ack) {
                // ack ok
                return nLen;
            } else {
                // ack failer
                LE("msg ack failed!!!!!,seq::%d, seq_ack:%d\n", m_msg._trans_seq, m_msg._trans_seq_ack);
                return nLen;
            }
        } else if (m_msg._action == TRANSFERACTION::resp) {
            TRANSFERMSG ack_msg;
            ack_msg._action = TRANSFERACTION::resp_ack;
            ack_msg._fmodule = TRANSFERMODULE::mmeeting;
            ack_msg._type   = m_msg._type;
            ack_msg._trans_seq = m_msg._trans_seq;
            ack_msg._trans_seq_ack = m_msg._trans_seq + 1;
            ack_msg._valid = m_msg._valid;
            ack_msg._content = "";
            const std::string s = ack_msg.ToJson();
            OnTransfer(s);
        } else if (m_msg._action == TRANSFERACTION::resp_ack) {
            if (m_msg._trans_seq + 1 == m_msg._trans_seq_ack) {
                // ack ok
                return nLen;
            } else {
                // ack failer
                LE("msg ack failed!!!!!,seq::%d, seq_ack:%d\n", m_msg._trans_seq, m_msg._trans_seq_ack);
                return nLen;
            }
        } else {
            LE("msg action error!!!!!\n");;
        }
    }
    {
        if (m_msg._type == TRANSFERTYPE::conn) {
            OnTypeConn(m_msg._fmodule, m_msg._content);
        } else if  (m_msg._type == TRANSFERTYPE::trans) {
            OnTypeTrans(m_msg._fmodule, m_msg._content);
        } else if (m_msg._type == TRANSFERTYPE::queue) {
            OnTypeQueue(m_msg._fmodule, m_msg._content);
        } else if (m_msg._type == TRANSFERTYPE::dispatch) {
            OnTypeDispatch(m_msg._fmodule, m_msg._content);
        } else if (m_msg._type == TRANSFERTYPE::push) {
            OnTypePush(m_msg._fmodule, m_msg._content);
        } else {
            LE("invalid type::%d\n", m_msg._type);
        }
    }
    return nLen;
}

int RTTransfer::ProcessData(const char* pData, int nLen)
{
    int parsed = 0;
    int ll = 0;

    while (parsed < nLen)
    {
        const char* pMsg = pData + parsed;
        int offset = 0;
        if (*(pMsg+offset) == '$') {
            offset += 1;
            char l[5] = {0};
            memset(l, 0x00, 5);
            memcpy(l, pMsg+offset, 4);
            offset += 4;
            ll = (int)strtol(l, NULL, 10);
            if (ll>=0 && ll <= nLen) {  // the message length may be 0
                int nlen = DoProcessData((char *)(pMsg+offset), ll);
                std::cout << "RTTransfer::ProcessData Msg======>>>>msg nlen:" << nlen << ", nlen:" << ", ll:" << ll << ", l:" << l << std::endl;
                if (nlen < 0) {
                    break;
                } else { // nlen < 0
                    offset += ll;
                    parsed += offset;
                }
            } else { // ll>0 && ll <= nLen
                LE("RTTransfer::ProcessData Get Msg Len Error!!!, ll:%d, nLen:%d, parsed:%d\n", ll, nLen, parsed);
            }
        }
    }
    std::cout << "RTTransfer::ProcessData RECV nLen:" << nLen << ", parsed:" << parsed << std::endl;
    return parsed;
}
