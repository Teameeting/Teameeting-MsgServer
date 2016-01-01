//
//  RTMessage.h
//  dyncRTMeeting
//
//  Created by hp on 12/2/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTMeeting_RTMessage_h
#define dyncRTMeeting_RTMessage_h

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
    /*from module, this is for transfer to idenfy where come from*/
    TRANSFERMODULE  _fmodule;
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
    // 1-connection lost
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
    std::list<const std::string> _us;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}TOJSONUSER;

#endif // dyncRTMeeting_RTMessage_h
