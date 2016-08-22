#ifndef _IOS_PUSHER_H_
#define _IOS_PUSHER_H_

#include "RTSingleton.h"

#include <capn/apn.h>
#include <capn/apn_array.h>
#include <capn/apn_payload.h>
#include <capn/apn_strings.h>
#include <capn/apn_strerror.h>

class IosPusher : public RTSingleton<IosPusher>{
    friend class RTSingleton<IosPusher>;

    public:
    static void ApnLogging(apn_log_levels level, const char* const message, uint32_t len);
    static void ApnInvalidToken(const char* const token, uint32_t index);

    public:
    bool InitPusher();
    bool UninPusher();

    bool ConfigToken();
    bool ConfigPayload();

    bool PushMsg();


    protected:
    IosPusher(){}
    ~IosPusher(){}

    private:
    apn_ctx_t               *mApnCtx;
    apn_payload_t           *mPayload;
    apn_array_t             *mTokens;
};

#endif // _IOS_PUSHER_H_
