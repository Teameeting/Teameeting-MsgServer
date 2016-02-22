//
//  RTMsg.h
//  dyncRTConnector
//
//  Created by hp on 12/10/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTConnector_RTMsg_h
#define dyncRTConnector_RTMsg_h


typedef enum _msgtype{
    meeting=3,
    callcenter,
    p2p,
    msgtype_invalid
}MSGTYPE;

typedef enum _messagetype{
    request=1,
    response,
    messagetype_invalid
}MESSAGETYPE;

typedef enum _meetcmd{
    enter=1,
    leave,
    dcomm,
    meetcmd_invalid
}MEETCMD;

typedef enum _dcommaction{
    msend=1,
    dsett,
    share,
    dcommaction_invalid
}DCOMMACTION;

typedef enum _sendtags{
    sendtags_talk=1,
    sendtags_enter,
    sendtags_leave,
    sendtags_subscribe,
    sendtags_unsubscribe,
    sendtags_audioset,
    sendtags_videoset,
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

typedef enum _getcmd{
    getcmd_invalid=1
}GETCMD;

#endif
