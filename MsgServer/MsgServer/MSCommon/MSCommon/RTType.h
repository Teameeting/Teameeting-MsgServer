//
//  RTType.h
//  dyncRTConnector
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef dyncRTConnector_RTType_h
#define dyncRTConnector_RTType_h

typedef enum _connectiontype{
    _chttp = 1,
    _ctcp,
    connectiontype_invalid
}CONNECTIONTYPE;

typedef struct _EventData{
    int mtype;
    union {
        struct {
            int         module;
            char        ip[16];
            int         port;
        }connect;
    };
}EventData;

typedef enum _sessevent{
    _sess_lost = 1
}SESSEVENT;

#endif
