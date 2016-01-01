//
//  RTMessage.h
//  dyncRTMsgClient
//
//  Created by hp on 12/2/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTMsgClient_RTMessage_h
#define dyncRTMsgClient_RTMessage_h

#include <iostream>
#include <list>

typedef enum _transferaction{
    req = 1,
    req_ack,
    resp,
    resp_ack,
    transferaction_invalid
}TRANSFERACTION;

typedef enum _transfermodule{
    mconnector = 1,
    mmsgqueue,
    mmeeting,
    mcallcenter,
    mp2p,
    transfermodule_invalid
}TRANSFERMODULE;

typedef enum _transfertype{
    conn = 1,
    trans,
    queue,
    dispatch,
    push,
    transfertype_invalid
}TRANSFERTYPE;

typedef enum _conntag{
    co_msg = 1,
    co_id,
    co_msgid,
    co_keepalive,
    conntype_invalid
}CONNTAG;

typedef struct _transfermsg{
    TRANSFERACTION  _action;
    TRANSFERMODULE  _fmodule;    /*from module*/
    TRANSFERTYPE    _type;
    long long       _trans_seq;
    long long       _trans_seq_ack;
    short           _valid;
    std::string     _content;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}TRANSFERMSG;

typedef struct _connmsg{
    CONNTAG         _tag;
    std::string     _msg;
    std::string     _id;
    std::string     _msgid;
    std::string     _moduleid;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}CONNMSG;

typedef struct _transmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}TRANSMSG;

typedef struct _queuemsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}QUEUEMSG;

typedef struct _dispatchmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}DISPATCHMSG;

typedef struct _pushmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}PUSHMSG;

typedef struct _tojsonuser{
    std::list<std::string> _us;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}TOJSONUSER;

#endif // dyncRTConnector_RTMessage_h
