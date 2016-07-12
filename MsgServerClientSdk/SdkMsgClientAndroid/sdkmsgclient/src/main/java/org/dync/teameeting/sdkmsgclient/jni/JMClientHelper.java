package org.dync.teameeting.sdkmsgclient.jni;

/**
 * Created by hp on 12/24/15.
 */
public interface JMClientHelper {
    public void OnSndMsg(int code, String msgid);
    public void OnCmdCallback(int code, int cmd, String groupid, String data);
    public void OnRecvMsg(long seqn, String msg);
    public void OnRecvGroupMsg(long seqn, String seqnid, String msg);
    public void OnSyncSeqn(long seqn, int role);
    public void OnSyncGroupSeqn(String groupid, long seqn);

    public void OnGroupNotify(int code, String seqnid);
    public void OnNotifySeqn(int code, String seqnid);
    public void OnNotifyData(int code, String seqnid);

    public void OnMsgServerConnected();
    public void OnMsgServerConnecting();
    public void OnMsgServerDisconnect();
    public void OnMsgServerConnectionFailure();
}
