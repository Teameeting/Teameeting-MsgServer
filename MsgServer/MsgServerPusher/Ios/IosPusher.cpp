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
    if (nullptr == mApnCtx)
    {
        fprintf(stderr, "Unable to init context:%d\n", errno);
        apn_library_free();
        return false;
    }
    apn_set_behavior(mApnCtx, APN_OPTION_RECONNECT);
    apn_set_mode(mApnCtx, APN_MODE_SANDBOX);
    apn_set_log_callback(mApnCtx, IosPusher::ApnLogging);
    apn_set_invalid_token_callback(mApnCtx, IosPusher::ApnInvalidToken);
    apn_set_pkcs12_file(mApnCtx, "./cert/com.dync.SdkMsgClientIos/dev/com_dync_SdkMsgClientIos_dev.p12", "123456");
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
    if (mTokens)
    {
        apn_array_free(mTokens);
    }
    apn_library_free();
}

bool IosPusher::ConfigToken()
{
    mTokens = apn_array_init(1, NULL, NULL);
    if (!mTokens)
    {
        fprintf(stderr, "Unable to init apn_array:%d\n", errno);
        if (mApnCtx)
            apn_free(mApnCtx);
        apn_library_free();
        return false;
    }


    apn_array_insert(mTokens, (void*)"fbeab7b7599e6910ae89af7103376483434a575da0be51e6e46b5a5920e76af6");

    if (APN_ERROR == apn_connect(mApnCtx))
    {
         printf("Could not connected to Apple Push Notification Service:%s, errno:%d\n", apn_error_string(errno), errno);
         if (mApnCtx)
             apn_free(mApnCtx);
         if (mTokens)
             apn_array_free(mTokens);
         apn_library_free();
         return false;
    }
}

bool IosPusher::ConfigPayload()
{
    mPayload = apn_payload_init();
    if (nullptr == mPayload)
    {
        fprintf(stderr, "Unable to init payload:%d\n", errno);
        if (mApnCtx)
            apn_free(mApnCtx);
        apn_library_free();
        return false;
    }
    time_t time_now = 0;
    time(&time_now);

    apn_payload_set_badge(mPayload, 10); // Icon badge
    //apn_payload_set_body(mPayload, "Test Push Message"); // notification text
    apn_payload_set_expiry(mPayload, time_now + 3600); // expire
    apn_payload_set_priority(mPayload, APN_NOTIFICATION_PRIORITY_HIGH);
    apn_payload_add_custom_property_integer(mPayload, "custom_property_integer", 100); // Custom property

}

int i = 0;
bool IosPusher::PushMsg()
{
    char msg[10] = {0};
    sprintf(msg, "%d msg pushed", i++);
    apn_payload_set_body(mPayload, msg); // notification text
    apn_array_t *invalid_tokens = NULL;
    int ret = 0;
    if (APN_ERROR == apn_send(mApnCtx, mPayload, mTokens, &invalid_tokens)) {
        printf("Could not send push: %s (errno: %d)\n", apn_error_string(errno), errno);
        ret = -1;
    } else {
        printf("Notification was successfully sent to %u device(s)\n",
               apn_array_count(mTokens) - ((invalid_tokens) ? apn_array_count(invalid_tokens) : 0));
        if (invalid_tokens) {
            printf("Invalid tokens:\n");
            uint32_t i = 0;
            for (; i < apn_array_count(invalid_tokens); i++) {
                printf("    %u. %s\n", i, apn_array_item_at_index(invalid_tokens, i));
            }
            apn_array_free(invalid_tokens);
        }
    }
}

