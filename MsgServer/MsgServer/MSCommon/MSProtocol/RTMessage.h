//
//  RTMessage.h
//  dyncRTConnector
//
//  Created by hp on 12/2/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTConnector_RTMessage_h
#define dyncRTConnector_RTMessage_h

#include <iostream>
#include <string>
#include <list>
#include "RTMsg.h"

typedef struct _transfermsg TRANSFERMSG;
struct _transfermsg{
    TRANSFERACTION  _action;
    /*from module, this is for transfer to idenfy where come from*/
    TRANSFERMODULE  _fmodule;
    TRANSFERTYPE    _type;
    uint64_t        _trans_seq;
    uint64_t        _trans_seq_ack;
    int             _valid;
    std::string     _content;
    _transfermsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _connmsg CONNMSG;
struct _connmsg{
    CONNTAG         _tag;
    std::string     _msg;
    std::string     _id;
    std::string     _msgid;
    std::string     _moduleid;
    _connmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _transmsg TRANSMSG;
struct _transmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    _transmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _queuemsg QUEUEMSG;
struct _queuemsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    _queuemsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _dispatchmsg DISPATCHMSG;
struct _dispatchmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    _dispatchmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _pushmsg PUSHMSG;
struct _pushmsg{
    int             _flag;
    std::string     _touser;
    std::string     _connector;
    std::string     _content;
    _pushmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _topushmsg TOPUSHMSG;
struct _topushmsg{
    int             _tags;
    std::string     _roomid;
    _topushmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _tojsonuser TOJSONUSER;
struct _tojsonuser{
    std::list<std::string> _us;
    _tojsonuser();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _topushuser TOPUSHUSER;
struct _topushuser{
    std::list<std::string> _us;
    _topushuser();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};


#endif // dyncRTConnector_RTMessage_h
