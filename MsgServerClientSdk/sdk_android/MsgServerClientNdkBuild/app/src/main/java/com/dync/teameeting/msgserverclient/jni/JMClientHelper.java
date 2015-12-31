package com.dync.teameeting.msgserverclient.jni;

/**
 * Created by hp on 12/24/15.
 */
public interface JMClientHelper {
    public void OnLogin(int code);
    public void OnSndMsg(String msg);
    public void OnGetMsg(String from, String msg);
    public void OnLogout(int code);
}
