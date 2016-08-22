//
//  XPushMsgProcesser.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XPushMsgProcesser.h"

#include <iostream>
#include <string>


int XPushMsgProcesser::EncodePushGetDataNotify(std::string& outstr, pms::StorageMsg& msg, int module)
{
    pms::MsgReq req;
    pms::TransferMsg tmsg;
    m_packed.mutable_msgs(0)->MergeFrom(msg);
    //group33333
    req.set_svr_cmds(pms::EServerCmd::CPGETDATA);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(m_packed.SerializeAsString());

    tmsg.set_type(pms::ETransferType::TPUSH);
    tmsg.set_content(req.SerializeAsString());
    outstr = tmsg.SerializeAsString();
    m_packed.mutable_msgs(0)->Clear();
    return 0;
}


/////////////////////////////////////////////////////
///////////////////DECODE MESSAGE////////////////////
/////////////////////////////////////////////////////

int XPushMsgProcesser::DecodeRecvData(const char* pData, int nLen)
{
    const std::string strmsg(pData, nLen);
    pms::MsgRep resp;
    if (!resp.ParseFromString(strmsg)) {
        std::cout << "RecvData resp.ParseFromString error!" << std::endl;
    }
    switch (resp.svr_cmds()) {
        case pms::EServerCmd::CPGETDATA:
            DecodePGetData(resp.rsp_code(), resp.rsp_cont());
            break;
        default:
            std::cout << "invalid svr_cmds type:" << resp.svr_cmds() << std::endl;
            break;
    }
    return nLen;
}

int XPushMsgProcesser::DecodePGetData(int code, const std::string& cont)
{
    m_helper.OnPGetData(code, cont);
    return 0;
}
