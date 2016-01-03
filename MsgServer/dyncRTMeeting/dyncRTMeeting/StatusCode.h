//
//  StatusCode.h
//  dyncRTMeeting
//
//  Created by hp on 1/3/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef StatusCode_h
#define StatusCode_h

#include <string>

typedef enum _rtcomm_code{
    _ok = 0,
    _invparams,
    _errconninfo,
    _errmoduinfo,
    _errtojson,
    _nexistroom,
    _nexistmem,
    _existroom,
    _existmem,
    _invalid
}RTCommCode;

static std::string RTCommStatus[] = {
    "ok",
    "invparams",
    "errconninfo",
    "errmoduinfo",
    "errtojson",
    "nexistroom",
    "nexistmem",
    "existroom",
    "existmem",
    "invalid"
};

int GetRTCommCode(std::string& status) {
    if (status.length()==0) {
        return RTCommCode::_invalid;
    }
    if (strcmp(status.c_str(), "ok")==0) {
        return RTCommCode::_ok;
    } else if (strcmp(status.c_str(), "invparams")==0) {
        return RTCommCode::_invparams;
    } else if (strcmp(status.c_str(), "errconninfo")==0) {
        return RTCommCode::_errconninfo;
    } else if (strcmp(status.c_str(), "errmoduinfo")==0) {
        return RTCommCode::_errmoduinfo;
    } else if (strcmp(status.c_str(), "errtojson")==0) {
        return RTCommCode::_errtojson;
    } else if (strcmp(status.c_str(), "nexistroom")==0) {
        return RTCommCode::_nexistroom;
    } else if (strcmp(status.c_str(), "nexistmem")==0) {
        return RTCommCode::_nexistmem;
    } else if (strcmp(status.c_str(), "existroom")==0) {
        return RTCommCode::_existroom;
    } else if (strcmp(status.c_str(), "existmem")==0) {
        return RTCommCode::_existmem;
    } else {
        return RTCommCode::_invalid;
    }
}

std::string GetRTCommStatus(int code) {
    return RTCommStatus[code];
}



#endif /* StatusCode_h */
