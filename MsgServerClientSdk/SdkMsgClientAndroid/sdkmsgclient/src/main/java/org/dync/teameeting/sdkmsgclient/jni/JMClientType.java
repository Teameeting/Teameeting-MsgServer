package org.dync.teameeting.sdkmsgclient.jni;

/**
 * Created by hp on 12/24/15.
 */
public class JMClientType {
    //* Connection Status
    public static final int CSNOT_CONNECTED = 0;
    public static final int CSCONNECTTING = 1;
    public static final int CSCONNECTED = 2;

    //* MsgClient Opt Room Cmd
    public static final int MCCMD_ENTER = 1;
    public static final int MCCMD_LEAVE = 2;
    public static final int MCCMD_DCOMM = 3;
    public static final int MCMD_MEETCMD_INVALID = 4;

    //* MsgClient send msg tags
    public static final int MCSENDTAGS_TALK=1;
    public static final int MCSENDTAGS_ENTER=2;
    public static final int MCSENDTAGS_LEAVE=3;
    public static final int MCSENDTAGS_CALL=4;
    public static final int MCSENDTAGS_INVALID=5;

    //* MsgClient message type
    public static final int MCMESSAGETYPE_REQUEST=1;
    public static final int MCMESSAGETYPE_RESPONSE=2;
    public static final int MCMESSAGETYPE_INVALID=3;

    //* MsgClient get msg cmd
    public static final int MCCMD_GET_INVALID=1;
}
