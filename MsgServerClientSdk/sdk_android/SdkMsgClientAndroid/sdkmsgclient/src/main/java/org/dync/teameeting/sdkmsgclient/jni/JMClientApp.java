package org.dync.teameeting.sdkmsgclient.jni;


/**
 * Created by hp on 12/24/15.
 */
public class JMClientApp {
    /**
     * 构造访问jni底层库的对象
     */

    private final long fNativeAppId;

    public JMClientApp(JMClientHelper helper) {
        fNativeAppId = Create(helper);
    }

    private static native long Create(JMClientHelper helper);

    public native int ConnStatus();
    public native int Init(String strUid, String strToken, String strNname, String strServer, int port);
    public native int Unin();

    public native int SndMsg(String strRoomid, String strRname, String strMsg);
    public native int GetMsg(int cmd);
    public native int OptRoom(int cmd, String strRoomid, String strRname, String strRemain);
    public native int SndMsgTo(String strRoomid, String strRname, String strMsg, String[] arrUser);

    public native int NotifyMsg(String strRoomid, String strRname, int tags, String strMsg);
    public native void SetNickName(String strNname);

    /**
     * 销毁APP
     */
    public native void Destroy();
}
