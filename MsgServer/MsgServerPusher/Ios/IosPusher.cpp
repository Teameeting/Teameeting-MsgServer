#include "IosPusher.h"
#include <assert.h>

void IosPusher::ApnLogging(apn_log_levels level, const char* const message, uint32_t len)
{
    const char *prefix = NULL;
    switch(level) {
        case APN_LOG_LEVEL_INFO:
            prefix = "inf";
            break;
        case APN_LOG_LEVEL_ERROR:
            prefix = "err";
            break;
        case APN_LOG_LEVEL_DEBUG:
            prefix = "dbg";
            break;
    }

    printf("======> [apn][%s]: %s\n", prefix, message);
}

void IosPusher::ApnInvalidToken(const char* const token, uint32_t index)
{
    printf("======> Invalid token: %s (index: %d)\n", token, index);
}

bool IosPusher::InitPusher()
{
    assert(apn_library_init() == APN_SUCCESS);
    mApnCtx = apn_init();
    if (!mApnCtx)
    {
        fprintf(stderr, "Unable to init context:%d\n", errno);
        apn_library_free();
        return false;
    }
    apn_set_behavior(mApnCtx, APN_OPTION_RECONNECT);
    apn_set_mode(mApnCtx, APN_MODE_SANDBOX);
    apn_set_log_callback(mApnCtx, IosPusher::ApnLogging);
    apn_set_invalid_token_callback(mApnCtx, IosPusher::ApnInvalidToken);
    //apn_set_pkcs12_file(mApnCtx, "./cert/com.dync.SdkMsgClientIos/dev/com_dync_SdkMsgClientIos_dev.p12", "123456");
    apn_set_pkcs12_file(mApnCtx, "/usr/local/dync/msgserver/pusher/bin/cert/com.avcon.BuddhismLiving/dev/com_avcon_BuddhismLiving_dev.p12", "123456");

    return true;
}

bool IosPusher::UninPusher()
{
    if (mApnCtx)
    {
        apn_free(mApnCtx);
    }
    if (mPayload)
    {
        apn_payload_free(mPayload);
    }
    apn_library_free();
    return true;
}

bool IosPusher::ConnectApns()
{
    if (!mApnCtx) return false;
    apn_close(mApnCtx);
    if (APN_ERROR == apn_connect(mApnCtx))
    {
        printf("Could not connected to Apple Push Notification Service:%s, errno:%d\n", apn_error_string(errno), errno);
        return false;
    } else {
        printf("connected to Apple Push Notification Service ok\n");
        return true;
    }
}

bool IosPusher::InitConfig()
{


    mPayload = apn_payload_init();
    if (!mPayload)
    {
        fprintf(stderr, "Unable to init payload:%d\n", errno);
        if (mApnCtx)
            apn_free(mApnCtx);
        apn_library_free();
        return false;
    }

    return true;
}

bool IosPusher::PushMsg(const char* token, const char* cont, const char** arr, uint32_t size)
{
    if (!token || !cont || !arr) return false;
    apn_array_t *tmp_tokens = apn_array_init(1, NULL, NULL);
    if (!tmp_tokens)
    {
        fprintf(stderr, "Unable to init apn_array:%d\n", errno);
        return false;
    }
    time_t time_now = 0;
    time(&time_now);
    apn_array_insert(tmp_tokens, (void*)token);
    apn_payload_set_badge(mPayload, 0); // Icon badge
    apn_payload_set_expiry(mPayload, time_now + 3600*4); // expire 4 hours
    apn_payload_set_priority(mPayload, APN_NOTIFICATION_PRIORITY_HIGH);
    apn_payload_set_body(mPayload, cont); // notification text
    for (int i=0;i<size;)
    {
        printf("arr key:%s, arr value:%s\n", arr[i], arr[i+1]);
        apn_payload_add_custom_property_string(mPayload, arr[i], arr[i+1]); // Custom property
        i += 2;
        if ((size-i)%2 != 0)
        {
            printf("The left element are not pairs, i:%d, size:%d, so break\n", i,  size);
            break;
        }
    }
    apn_array_t *invalid_tokens = NULL;
    bool ret = true;
    if (APN_ERROR == apn_send(mApnCtx, mPayload, tmp_tokens, &invalid_tokens)) {
        printf("Could not send push: %s (errno: %d)\n", apn_error_string(errno), errno);
        ConnectApns();
        ret = false;
    } else {
        printf("Notification was successfully sent to %u device(s)\n",
               apn_array_count(tmp_tokens) - ((invalid_tokens) ? apn_array_count(invalid_tokens) : 0));
        if (invalid_tokens) {
            printf("Invalid tokens:\n");
            uint32_t i = 0;
            for (; i < apn_array_count(invalid_tokens); i++) {
                printf("    %u. %s\n", i, apn_array_item_at_index(invalid_tokens, i));
            }
            apn_array_free(invalid_tokens);
            ConnectApns();
            ret = false;
        }
    }
    if (tmp_tokens)
    {
        apn_array_free(tmp_tokens);
    }
    return ret;
}

