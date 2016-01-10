//
//  RTMsg.h
//  dyncRTMsgClient
//
//  Created by hp on 12/10/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTMsgClient_RTMsg_h
#define dyncRTMsgClient_RTMsg_h


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

#endif // dyncRTMsgClient_RTMsg_h
