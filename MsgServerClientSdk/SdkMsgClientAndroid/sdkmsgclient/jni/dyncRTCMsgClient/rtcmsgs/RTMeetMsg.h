//
//  RTMeetMsg.h
//  dyncRTCMsgClient
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTCMsgClient_RTMeetMsg_h
#define dyncRTCMsgClient_RTMeetMsg_h

/*
 *  Http:
 *  Method:     GET     POST
 *  Header:     /login      /logout     /getmsg     /sndmsg
 *  Body:       {}
 *      login:
 *          from: userid
 *          pass: pass
 *      logout:
 *          from: userid
 *      getmsg:
 *          from: userid
 *      sndmsg: MSG
 *          MSG:
 *              mseq:num
 *              code:result code
 *              status:result
 *              mtype: meet, callcenter, p2p
 *              nmem: the number of members
 *              ntime: the time of this message
 *              meet:
 *                  cmd: enter, leave, dcomm
 *                      enter:
 *                          from: userid
 *                          room: roomid
 *                      leave:
 *                          from: userid
 *                          room: roomid
 *                      dcomm:
 *                          from: userid
 *                          room: roomid
 *                          to: a, u
 *                              a:[]
 *                              u:[]
 *                          action: send, dsett, share
 *                              send:
 *                                  tags: talk, enter, leave, subscribe, unsubscribe,invalid
 *                                  type: msg, file, pic, emoj, aud, vid
 *                                  content: {}
 *                              dsett:
 *                                  type: gname
 *                                  content: {}
 *                              share:
 *                                  type: scrn
 *                                  content: {}
 *              callcenter:
 *              p2p:
 *
 *      MSG:
 *          {
 *              required  mseq:    {num}
 *              required  mtype:   [meet,callcenter,p2p]
 *              required  cmd:     {}
 *              required  from:    {}
 *              required  room:    {}
 *              optional  to:      {a:[],u:[]}
 *              optional  action:  []
 *              optional  tags:    []
 *              optional  type:    []
 *              optional  content: {}
 *              optional  pass:    {}
 *              optional  code:    {}
 *              optional  status:  {}
 *              optional  nmem:    {}
 *              optional  ntime:   {}
 *          }
 *      e.g. mtype: meet
 *          {
 *              mseq:asdf1234id001
 *              mtype:meet
 *              cmd:dcomm
 *              from:test001
 *              room:asdf1234id
 *              to:{[u:test002]}
 *              action:send
 *              tags:talk
 *              type:msg
 *              content:helloworld
 *              pass:123
 *              code:200
 *              status:ok
 *              nmem:0
 *              ntime:0
 *          }
 *      interface
 *          /login          POST
 *              (required from,
 *               required pass)
 *          /logout         POST
 *              (required from)
 *          /getmsg         GET
 *              (required from)
 *          /sndmsg         POST
 *              (required mseq,
 *               required mtype,
 *               required cmd,
 *               required from,
 *               required room,
 *               optional to: {all,ulist:[]},
 *               optional action,
 *               optional tags,
 *               optional type,
 *               optional content,
 *               optional code,
 *               optional status,
 *               optional nmem,
 *               optional ntime)
 *
 *      if 'sess' is null, means you are not in any sessions.
 *      when 'action' is 'send', 'tags' means you are talking in the session or just chatting out of session with someone or maybe leaving a message to someone not online.
 */

#include <iostream>
#include "RTMsg.h"

typedef struct _meetmsg MEETMSG;
struct _meetmsg{
    MSGTYPE     _mtype;
    MESSAGETYPE _messagetype;
    int         _signaltype;
    int         _cmd;
    int         _action;
    int         _tags;
    int         _type;
    int         _nmem;
    int64_t     _ntime;
    uint64_t    _mseq;
    std::string _from;
    std::string _room;
    std::string _to;
    std::string _cont;
    std::string _pass;
    std::string _nname;/*nickname*/
    std::string _rname;/*roomname*/
    int         _code;
    _meetmsg();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

#endif  // dyncRTCMsgClient_RTMeetMsg_h
