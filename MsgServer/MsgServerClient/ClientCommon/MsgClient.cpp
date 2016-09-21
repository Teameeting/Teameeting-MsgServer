//
//  MsgClient.mm
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "MsgClient.h"
#include "MSSubMessage.h"

#if 0
#include "proto/common_msg.pb.h"
#include "proto/entity_msg.pb.h"
#include "proto/entity_msg_type.pb.h"
#else
#include "MsgServerClient/client_common/proto/common_msg.pb.h"
#include "MsgServerClient/client_common/proto/entity_msg.pb.h"
#include "MsgServerClient/client_common/proto/entity_msg_type.pb.h"
//#include "client_common/proto/common_msg.pb.h"
//#include "client_common/proto/entity_msg.pb.h"
//#include "client_common/proto/entity_msg_type.pb.h"
#endif

#include "MSMsgUtil.h"

int MsgClient::MCInit(const std::string& uid, const std::string& token, const std::string& nname)
{
    std::cout << "MCInit add module here, please implement it!!!" << std::endl;
    m_strUserId = uid;
    m_strToken = token;
    m_strNname = nname;
    m_nsUserId = uid;
    m_nsToken = token;
    m_nsNname = nname;

    // get UUID
    //CFUUIDRef uuid = CFUUIDCreate(kCFAllocatorDefault);
    //NSString *uuidStr = (__bridge_transfer NSString *)CFUUIDCreateString(kCFAllocatorDefault, uuid);
    //CFRelease(uuid);
    SetUUID("thisisuuidlaterusingtimereplace");

    m_isFetched = false;
    //m_recurLock = [[NSRecursiveLock alloc] init];
    m_sqlite3Manager = new MSSqlite3Manager();
    if (!m_sqlite3Manager) return -1;
    m_sqlite3Manager->InitManager();
    CheckUserOrInit(m_nsUserId);
    GetLocalSeqnsFromDb();
    SyncSeqnFromDb2Core();
    return Init(uid, token, nname, DEVICE_IOS, PUSH_ENABLE, pms::EModuleType::TLIVE);
}

int MsgClient::MCUnin()
{
    PutLocalSeqnsToDb();
    if (m_sqlite3Manager)
    {
        m_sqlite3Manager->UninManager();
        delete m_sqlite3Manager;
        m_sqlite3Manager = nullptr;
    }
    //if (m_recurLock)
    //{
    //    m_recurLock = nullptr;
    //}
    if (m_nsGroupInfo.size()>0)
    {
        m_nsGroupInfo.clear();
    }
    if (m_groupSeqn.size()>0)
        m_groupSeqn.clear();
    m_isFetched = false;
    return Unin();
}

int MsgClient::MCRegisterMsgCb(XMsgCallback* cb)
{
    return RegisterMsgCb(cb);
}

int MsgClient::MCUnRegisterMsgCb(XMsgCallback* cb)
{
    return UnRegisterMsgCb(cb);
}

int MsgClient::MCConnToServer(const std::string& server, int port)
{
    ConnToServer(server, port);
    return 0;
}


int MsgClient::MCAddGroup(const std::string& groupid)
{
    AddGroup(groupid);
    return 0;
}

int MsgClient::MCRmvGroup(const std::string& groupid)
{
    RmvGroup(groupid);
    return 0;
}

int MsgClient::MCSyncMsg()
{
    if (m_isFetched)
    {
        LI("MCSyncMsg sync all seqns");
        SyncAllSeqns();
    } else {
        LI("MCSyncMsg should be fetched before called");
        FetchAllSeqns();
    }
    return 0;
}

int MsgClient::MCSync2Db()
{
    PutLocalSeqnsToDb();
    return 0;
}

/////////////////////////////////////////////////
//////////////////Send Message///////////////////
/////////////////////////////////////////////////

int MsgClient::MCSendTxtMsg(std::string& outmsgid, MSTxtMessage *txtMsg)
{
    if (!txtMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithTxtMsg(txtMsg, MCMsgTypeTtxt);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsg JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }
    int ret = SndMsg(outmsgid, txtMsg->GetGroupId(), "roomname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}


int MsgClient::MCSendTxtMsgTos(std::string& outmsgid, MSTxtMessage *txtMsg, const std::vector<std::string>& vusrs)
{
    if (!txtMsg) return -3;
    if (vusrs.size()==0) return -4;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithTxtMsg(txtMsg, MCMsgTypeTtxt);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgTos JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }
    int ret = SndMsgTo(outmsgid, txtMsg->GetGroupId(), "roomname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FMULTI, mMsg->GetPush(), vusrs);
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCSendTxtMsgToUsr(std::string& outmsgid, MSTxtMessage *txtMsg)
{
    if (!txtMsg) return -3;
    std::vector<std::string> uvec;
    uvec.push_back(txtMsg->GetToId());
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithTxtMsg(txtMsg, MCMsgTypeTtxt);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = SndMsgTo(outmsgid, "groupid", "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FSINGLE, mMsg->GetPush(), uvec);
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCSendTxtMsgToUsrs(std::string& outmsgid, MSTxtMessage *txtMsg, const std::vector<std::string>& vusrs)
{
    if (!txtMsg) return -3;
    if (vusrs.size()==0) return -4;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithTxtMsg(txtMsg, MCMsgTypeTtxt);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsrs JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = SndMsgTo(outmsgid, "groupid", "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FMULTI, mMsg->GetPush(), vusrs);
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}


int MsgClient::MCNotifyLive(std::string& outmsgid, MSLivMessage *livMsg)
{
    if (!livMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithLivMsg(livMsg, MCMsgTypeTliv);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = SndMsg(outmsgid, livMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TLIV, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCNotifyRedEnvelope(std::string& outmsgid, MSRenMessage *renMsg)
{
    if (!renMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithRenMsg(renMsg, MCMsgTypeTren);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = SndMsg(outmsgid, renMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TREN, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCNotifyBlacklist(std::string& outmsgid, MSBlkMessage *blkMsg, const std::vector<std::string>& notifys)
{
    if (!blkMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithBlkMsg(blkMsg, MCMsgTypeTblk);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = 0;
    if (notifys.size()==0) {
        ret = SndMsg(outmsgid, blkMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TBLK, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    } else {

        std::vector<std::string> uvec;
        uvec.push_back(blkMsg->GetToId());
        for (auto &it : notifys) {
            uvec.push_back(it);
        }
        ret = SndMsgTo(outmsgid, blkMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TBLK, pms::EModuleType::TLIVE, pms::EMsgFlag::FMULTI, mMsg->GetPush(), uvec);
    }
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCNotifyForbidden(std::string& outmsgid, MSFbdMessage *fbdMsg, const std::vector<std::string>& notifys)
{
    if (!fbdMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithFbdMsg(fbdMsg, MCMsgTypeTfbd);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = 0;
    if (notifys.size()==0) {
       ret = SndMsg(outmsgid, fbdMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TFBD, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    } else {
        std::vector<std::string> uvec;
        uvec.push_back(fbdMsg->GetToId());
        for (auto &it : notifys) {
            uvec.push_back(it);
        }
        ret = SndMsgTo(outmsgid, fbdMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TFBD, pms::EModuleType::TLIVE, pms::EMsgFlag::FMULTI, mMsg->GetPush(), uvec);
    }
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

int MsgClient::MCNotifySettedMgr(std::string& outmsgid, MSMgrMessage *mgrMsg, const std::vector<std::string>& notifys)
{
    if (!mgrMsg) return -3;
    MSMessage *mMsg = MSMsgUtil::EncodeMessageWithMgrMsg(mgrMsg, MCMsgTypeTmgr);
    std::string *jsonString = MSMsgUtil::MessageToJson(*mMsg);

    if (!jsonString)
    {
        LI("MCSendTxtMsgToUsr JSONToNSString error");
        delete mMsg;
        mMsg = nullptr;
        return -3;
    }

    int ret = SndMsg(outmsgid, mgrMsg->GetGroupId(), "grpname", jsonString->c_str(), pms::EMsgTag::TCHAT, pms::EMsgType::TMGR, pms::EModuleType::TLIVE, pms::EMsgFlag::FGROUP, mMsg->GetPush());
    delete mMsg;
    mMsg = nullptr;
    delete jsonString;
    jsonString = nullptr;
    return ret;
}

/////////////////////////////////////////////////
//////////Callback From XMsgCallback/////////////
/////////////////////////////////////////////////

void MsgClient::OnSndMsg(int code, const std::string& msgid)
{
    std::cout << "MsgClient::OnSndMsg msgid:" << msgid << ", code:" << code << std::endl;
    if (m_subMsgDelegate)
        m_subMsgDelegate->OnSendMessage(msgid, code);
}

void MsgClient::OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data)
{
    LI("MsgClient::OnCmdCallback cmd:%d, groupid.length:%lu, result:%d, seqn:%lld", cmd, groupid.length(), data.result, data.seqn);
    switch (cmd)
    {
        case MSADD_GROUP:
        {
            if (code == 0)
            {
                LI("OnCmdCallback add group ok, insert groupid and seqn, toggle callback");
                if (!m_sqlite3Manager->IsGroupExists(m_nsUserId, groupid))
                {
                    m_sqlite3Manager->AddGroupId(m_nsUserId, groupid);
                }
                if (!m_sqlite3Manager->IsUserExists(m_nsUserId, groupid))
                {
                    m_sqlite3Manager->AddGroupSeqn(m_nsUserId, groupid, data.seqn);
                }
                UpdateLocalSeqn(groupid, data.seqn);
                UpdateSeqnFromDb2Core();
                m_groupDelegate->OnAddGroupSuccess(groupid);;
            } else if (code == -1)
            {
                m_groupDelegate->OnAddGroupFailed(groupid, data.data, code);
            }
        }
            break;
        case MSRMV_GROUP:
        {
            if (code == 0)
            {
                LI("OnCmdCallback del group ok, del groupid and seqn, toggle callback");
                m_sqlite3Manager->DelGroupId(m_nsUserId, groupid);
                RemoveLocalSeqn(groupid);
                //Update seqn from db 2 core
                m_groupDelegate->OnRmvGroupSuccess(groupid);;
            } else if (code == -1)
            {
                m_groupDelegate->OnRmvGroupFailed(groupid, data.data, code);
            }
        }
            break;
        case MSFETCH_SEQN:
        {
            if (groupid.length()==0) // for user
            {
                if (data.result==0)
                {
                    UpdateGroupInfoToDb(m_nsUserId, data.seqn, 1);
                } else if (data.result==-1)
                {
                    UpdateGroupInfoToDb(m_nsUserId, data.seqn, -1);
                } else if (data.result==-2)
                {
                    UpdateGroupInfoToDb(m_nsUserId, data.seqn, -2);
                }
            } else { // for group
                if (data.result==0)
                {
                    UpdateGroupInfoToDb(groupid, data.seqn, 1);
                    UpdateSeqnFromDb2Core();
                } else if (data.result==-1)
                {
                    UpdateGroupInfoToDb(groupid, data.seqn, -1);
                    //Update seqn from db 2 core
                } else if (data.result==-2)
                {
                    UpdateGroupInfoToDb(groupid, data.seqn, -2);
                    //Update seqn from db 2 core
                }
            }
        }
            break;
        default:
            break;
    }
}


void MsgClient::OnRecvMsg(int64 seqn, const std::string& msg)
{
    std::cout << "MsgClient::OnRecvMsg was called, seqn:" << seqn << ", msg.length:" << msg.length() << std::endl;

    UpdateLocalSeqn(m_strUserId, seqn);
    UpdateSeqnToDb(m_strUserId, seqn);

    pms::Entity entity;
    if (!entity.ParseFromString(msg)) {
        LI("entity ParseFromString failed error\n");
        return;
    }

    if (entity.usr_from().compare(m_nsUserId)==0) {
        LI("MsgClient::OnRecvMsg recv the msg you just send!!!, so return\n");
        return;
    }
    LI("MsgClient::OnRecvMsg entityByte tag:%d, cont:%s, romid:%s, usr_from:%s\n", entity.msg_tag(), entity.msg_cont().c_str(), entity.rom_id().c_str(), entity.usr_from().c_str());

    MSMessage *mMsg = MSMsgUtil::JsonToMessage(entity.msg_cont());
    mMsg->SetMillSec(entity.msg_time());
    mMsg->SetMsgId(entity.cmsg_id());

    switch (entity.msg_type())
    {
        case pms::EMsgType::TTXT:
        {
            m_subMsgDelegate->OnRecvTxtMessage(mMsg);
        }
            break;
        case pms::EMsgType::TFIL:
        {
        }
            break;
        case pms::EMsgType::TPIC:
        {
        }
            break;
        case pms::EMsgType::TAUD:
        {
        }
            break;
        case pms::EMsgType::TVID:
        {
        }
            break;
        case pms::EMsgType::TEMJ:
        {
        }
            break;
        case pms::EMsgType::TSDF:
        {
            m_subMsgDelegate->OnRecvSelfDefMessage(mMsg);
        }
            break;
        case pms::EMsgType::TLIV:
        {
        }
            break;
        case pms::EMsgType::TREN:
        {
        }
            break;
        case pms::EMsgType::TBLK:
        {
            m_subMsgDelegate->OnNotifyBlacklistMessage(mMsg);
        }
            break;
        case pms::EMsgType::TFBD:
        {
            m_subMsgDelegate->OnNotifyForbiddenMessage(mMsg);
        }
            break;
        case pms::EMsgType::TMGR:
        {
        }
            break;
        default:
        {
        }
            break;
    }
}

void MsgClient::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
    std::cout << "MsgClient::OnRecvGroupMsg was called, seqn:" << seqn << ", seqnid:" << seqnid << ", msg.length:" << msg.length() << std::endl;

    UpdateLocalSeqn(seqnid, seqn);
    UpdateSeqnToDb(seqnid, seqn);

    pms::Entity entity;;
    if (!entity.ParseFromString(msg)) {
        LI("eeeee Entity parseFromData has error\n");
        return;
    }

    if (entity.usr_from().compare(m_nsUserId)==0) {
        LI("MsgClient::OnRecvGroupMsg recv the msg you just send!!!, so return");
        return;
    }
    LI("MsgClient::OnRecvGroupMsg entity tag:%d, cont:%s, romid:%s, usr_from:%s", entity.msg_tag(), entity.msg_cont().c_str(), entity.rom_id().c_str(), entity.usr_from().c_str());

    MSMessage *mMsg = MSMsgUtil::JsonToMessage(entity.msg_cont());
    mMsg->SetMillSec(entity.msg_time());
    mMsg->SetMsgId(entity.cmsg_id());

    switch (entity.msg_type())
    {
        case pms::EMsgType::TTXT:
        {
            m_subMsgDelegate->OnRecvTxtMessage(mMsg);
        }
            break;
        case pms::EMsgType::TFIL:
        {
        }
            break;
        case pms::EMsgType::TPIC:
        {
        }
            break;
        case pms::EMsgType::TAUD:
        {
        }
            break;
        case pms::EMsgType::TVID:
        {
        }
            break;
        case pms::EMsgType::TEMJ:
        {
        }
            break;
        case pms::EMsgType::TSDF:
        {
            m_subMsgDelegate->OnRecvSelfDefMessage(mMsg);
        }
            break;
        case pms::EMsgType::TLIV:
        {
            m_subMsgDelegate->OnNotifyLiveMessage(mMsg);
        }
            break;
        case pms::EMsgType::TREN:
        {
            m_subMsgDelegate->OnNotifyRedEnvelopeMessage(mMsg);
        }
            break;
        case pms::EMsgType::TBLK:
        {
            m_subMsgDelegate->OnNotifyBlacklistMessage(mMsg);
        }
            break;
        case pms::EMsgType::TFBD:
        {
            m_subMsgDelegate->OnNotifyForbiddenMessage(mMsg);
        }
            break;
        case pms::EMsgType::TMGR:
        {
            m_subMsgDelegate->OnNotifySettedMgrMessage(mMsg);
        }
            break;
        default:
        {
        }
            break;
    }
}


//////////////Below Used in this class//////////////////////////
//////////////Later it will be removed//////////////////////////

void MsgClient::OnSyncSeqn(int64 maxseqn, int role)
{
    // if it is sender, this means client send msg, and just get the newmsg's seqn
    // if the new seqn is bigger 1 than cur seqn, it is ok, just update seqn.
    // if the new seqn is bigger 2 or 3 than cur seqn, this need sync data
    // if it is recver, this means client need sync data
    LI("MsgClient::OnSyncSeqn NOT IMPLEMENT!!!");
}

// get group max seqn
void MsgClient::OnSyncGroupSeqn(const std::string &groupid, int64 maxseqn)
{
    LI("MsgClient::OnSyncGroupSeqn NOT IMPLEMENT!!!");
}

// not implement
void MsgClient::OnGroupNotify(int code, const std::string& seqnid)
{
    LI("MsgClient::OnGroupNotify NOT IMPLEMENT!!!");
}

void MsgClient::OnNotifySeqn(int code, const std::string& seqnid)
{
    LI("MsgClient::OnNotifySeqn NOT IMPLEMENT!!!");
}

void MsgClient::OnNotifyData(int code, const std::string& seqnid)
{
    LI("MsgClient::OnNotifyData NOT IMPLEMENT!!!");
}

void MsgClient::OnNotifyOtherLogin(int code)
{
    LI("MsgClient::OnNotifyOtherLogin code:%d!!!", code);
    if (m_subMsgDelegate)
        m_subMsgDelegate->OnNotifyOtherLogin(code);
}

//////////////Above Used in this class//////////////////////////

void MsgClient::OnMsgServerConnected()
{
    std::cout << "MsgClient::OnMsgServerConnected was called, fetchallseqn once" << std::endl;
    FetchAllSeqns();
    m_clientDelegate->OnMsgServerConnected();
    m_clientDelegate->OnMsgClientInitializing();

#if 0
    // check per second until get all fetch
    NSTimeInterval period = 1.0; //设置时间间隔
    //dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_source_t _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    dispatch_source_set_timer(_timer, dispatch_walltime(NULL, 0), period * NSEC_PER_SEC, 0); //每秒执行
    dispatch_source_set_event_handler(_timer, ^{
        LI(@"MsgClient::OnMsgServerConnected timer was called...............");
        //在这里执行事件
        if (IsFetchedAll())
        {
            // stop timer here
            m_isFetched = true;
            UpdateSeqnFromDb2Core();
            SyncAllSeqns();
            [m_clientDelegate OnMsgClientInitialized];
            dispatch_source_set_cancel_handler(_timer, ^{

                LI(@"MsgClient::OnMsgServerConnected dispatch_source_cancel...ok");
            });
            dispatch_source_cancel(_timer);
        } else {
            FetchAllSeqns();
        }
    });
    dispatch_resume(_timer);
#endif
}


void MsgClient::OnMsgServerConnecting()
{
    //std::cout << "MsgClient::OnMsgServerConnecting was called" << std::endl;
    if (m_clientDelegate)
        m_clientDelegate->OnMsgServerConnecting();
}

void MsgClient::OnMsgServerDisconnect()
{
    //std::cout << "MsgClient::OnMsgServerDisconnect was called" << std::endl;
    if (m_clientDelegate)
        m_clientDelegate->OnMsgServerDisconnect();
}

void MsgClient::OnMsgServerConnectionFailure()
{
    //std::cout << "MsgClient::OnMsgServerConnectionFailer was called" << std::endl;
    if (m_clientDelegate)
        m_clientDelegate->OnMsgServerConnectionFailure();
}
