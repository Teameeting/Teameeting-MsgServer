//
//  RTTransfer.cpp
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
    TRANSFERMSG m_msg;

    std::string str(pData, nLen), err("");
    m_msg.GetMsg(str, err);
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
    {
        if (m_msg._action == TRANSFERACTION::req || m_msg._action == TRANSFERACTION::resp) {
            OnMsgAck(m_msg);
        } else if (m_msg._action == TRANSFERACTION::req_ack || m_msg._action == TRANSFERACTION::resp_ack) {
            return nLen;
        } else {
            LE("msg action error!!!!!\n");
            return nLen;
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
            LE("invalid type::%d", m_msg._type);
        }
    }
    return nLen;
}
