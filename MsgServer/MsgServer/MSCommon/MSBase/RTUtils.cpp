//
//  RTUtils.cpp
//  MsgServer
//
//  Created by hp on 1/29/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "RTUtils.hpp"
#include "StrPtrLen.h"
#include "md5.h"
#include "md5digest.h"
#include "OS.h"
#include "atomic.h"

static unsigned int g_trans_id = 0;

void GenericSessionId(std::string& strId)
{
    
    SInt64 curTime = 0;
    MD5_CTX context;
    StrPtrLen hashStr;
    char          curMicroSecStr[33] = {0};
    unsigned char digest[16] = {0};
    {
        curTime = OS::Milliseconds();
        qtss_sprintf(curMicroSecStr, "%lld", curTime);
        MD5_Init(&context);
        MD5_Update(&context, (unsigned char*)curMicroSecStr, (unsigned int)strlen((const char*)curMicroSecStr));
        MD5_Final(digest, &context);
        HashToString(digest, &hashStr);
        strId = hashStr.Ptr;
        hashStr.Delete();
    }
}

unsigned int GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}