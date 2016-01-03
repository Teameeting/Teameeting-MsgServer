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
 *              mseq:room+num
 *              code:result code
 *              status:result
 *              mtype: meet, callcenter, p2p
 *              nmem: the number of members
 *              ntime: the time of this message
 *              meet:
 *                  cmd: enter, leave, dcomm, create, destroy, refresh
 *                      enter:
 *                          from: userid
 *                          room: roomid
 *                          sess: sessid
 *                      leave:
 *                          from: userid
 *                          room: roomid
 *                          sess: sessid
 *                      dcomm:
 *                          from: userid
 *                          room: roomid
 *                          sess: sessid
 *                          to: a, u
 *                              a:[]
 *                              u:[]
 *                          action: send, dsett, share
 *                              send:
 *                                  tags: talk, chat, lvmsg
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
 *              required  mseq:    {room+num}
 *              required  mtype:   [meet,callcenter,p2p]
 *              required  cmd:     {}
 *              required  from:    {}
 *              required  room:    {}
 *              optional  sess:    {}
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
 *              sess:1234asdf
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
 *               optional sess,
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

typedef enum _meetcmd{
    enter=1,
    leave,
    create,
    destroy,
    dcomm,
    refresh,
    meetcmd_invalid
}MEETCMD;

typedef enum _dcommaction{
    msend=1,
    dsett,
    share,
    dcommaction_invalid
}DCOMMACTION;

typedef enum _sendtags{
    talk=1,
    chat,
    lvmsg,
    sendtags_invalid
}SENDTAGS;

typedef enum _sendtype{
    msg=1,
    file,
    pic,
    emoj,
    aud,
    vid,
    sendtype_invalid
}SENDTYPE;

typedef enum _dsetttype{
    gname=1,
    dsetttype_invalid
}DSETTTYPE;

typedef enum _sharetype{
    scrn=1,
    sharetype_invalid
}SHARETYPE;

typedef struct _meetmsg{
    MSGTYPE     _mtype;
    int         _cmd;
    int         _action;
    int         _tags;
    int         _type;
    int         _nmem;
    long long   _ntime;
    long long   _mseq;
    std::string _from;
    std::string _room;
    std::string _sess;
    std::string _to;
    std::string _cont;
    std::string _pass;
    int         _code;
    std::string _status;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}MEETMSG;

#endif  // dyncRTCMsgClient_RTMeetMsg_h