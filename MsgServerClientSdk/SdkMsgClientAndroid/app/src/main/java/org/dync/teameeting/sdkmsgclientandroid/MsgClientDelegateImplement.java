package org.dync.teameeting.sdkmsgclientandroid;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import org.dync.teameeting.sdkmsgclient.msgs.MSClientDelegate;
import org.dync.teameeting.sdkmsgclient.msgs.MSClientManager;
import org.dync.teameeting.sdkmsgclient.msgs.MSGroupDelegate;
import org.dync.teameeting.sdkmsgclient.msgs.MSGroupManager;
import org.dync.teameeting.sdkmsgclient.msgs.MSMessageManager;
import org.dync.teameeting.sdkmsgclient.msgs.MSTxtMessage;
import org.dync.teameeting.sdkmsgclient.msgs.MSTxtMessageDelegate;


/**
 * Created by hp on 7/12/16.
 */
public class MsgClientDelegateImplement implements MSClientDelegate, MSGroupDelegate, MSTxtMessageDelegate{

    private final String        MSG_CLIENT_TAG = "MsgClientTag";
    private MSClientManager     mMsClientManager    = null;
    private MSGroupManager      mMsGroupManager     = null;
    private MSMessageManager    mMsMessageManager   = null;
    private String              mStrUserId = null;
    private String              mStrToken = null;
    private String              mStrNname = null;
    private String              mServer = "192.168.7.207";
    private int                 mPort = 6630;

    private Context             mContext = null;
    private Handler             mHandler = null;

    public MsgClientDelegateImplement(Context context, Handler handler)
    {
        mContext = context;
        mHandler = handler;
        mMsClientManager = new MSClientManager();
        mMsClientManager.initMsgClient(mContext, mStrUserId, mStrToken, mStrNname);
        mMsGroupManager = new MSGroupManager();
        mMsMessageManager = new MSMessageManager();

        mMsClientManager.addDelegate(this);
        mMsGroupManager.addDelegate(this);
        mMsMessageManager.addDelegate(this);

        mMsClientManager.connToServer(mServer, mPort);
    }

    public void MsgClientDestroy() {
        mMsMessageManager.delDelegate(this);
        mMsGroupManager.delDelegate(this);
        mMsClientManager.delDelegate(this);
        mMsClientManager.uninMsgClient();
    }

    public void EnterRoom() {
        System.out.println("EnterRoom was called...");
        String groupid = "wocaowocaowocao";
        mMsGroupManager.addGroup(groupid);
    }

    public void SendMsgTo() {
        System.out.println("SendMsgTo was called...");

        String cont = "hahaha";
        String touser = "";
        String outmsgid = mMsMessageManager.sendTxtMsgTo(touser, cont);
        System.out.println("SendMsgTo sendTxtMsgTo get msgid:"+outmsgid);
    }

    public void LeaveRoom() {
        System.out.println("LeaveRoom was called...");
        String groupid = "wocaowocaowocao";
        mMsGroupManager.rmvGroup(groupid);
    }

    public void NotifyMsg() {
        System.out.println("NotifyMsg was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        String outmsgid = mMsMessageManager.sendTxtMsg(groupid, cont);
        System.out.println("NotifyMsg sendTxtMsg get msgid:"+outmsgid);
    }


    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////Interface Implement/////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////


    @Override
    public void OnMsgServerConnected() {
            String s = "OnMsgServerConnected was called";
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
    }

    @Override
    public void OnMsgServerConnecting() {

    }

    @Override
    public void OnMsgServerDisconnect() {
            String s = "OnMsgServerDisconnect was called";
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
    }

    @Override
    public void OnMsgServerConnectionFailure() {
        String s = "OnMsgServerConnectionFailure was called";
        System.out.println(s);
        Message tMsg = Message.obtain();
        tMsg.what = 1;
        tMsg.obj = s;
        mHandler.sendMessage(tMsg);
    }

    @Override
    public void OnMsgClientInitializing() {

    }

    @Override
    public void OnMsgClientInitialized() {

    }

    @Override
    public void OnMsgClientUnInitialize() {

    }

    @Override
    public void OnMsgClientInitializeFailure() {

    }

    @Override
    public void OnAddGroupSuccess(String grpId) {

    }

    @Override
    public void OnAddGroupFailed(String grpId, String reason, int code) {

    }

    @Override
    public void OnRmvGroupSuccess(String grpId) {

    }

    @Override
    public void OnRmvGroupFailed(String grpId, String reason, int code) {

    }

    @Override
    public void OnSendMessage(String msgId, int code) {

    }

    @Override
    public void OnRecvTxtMessage(MSTxtMessage txtMsg) {
        String s = "OnSndMsg msg:" + txtMsg;
        System.out.println(s);
        String content = txtMsg.getContent();
        if (content != null) {
            Log.i(MSG_CLIENT_TAG, content);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = content;
            mHandler.sendMessage(tMsg);
        }
    }

    @Override
    public void OnRecvSelfDefMessage(MSTxtMessage sdefMsg) {

    }

    @Override
    public void OnNotifyLive(String grpId, String hostId) {

    }

    @Override
    public void OnNotifyRedEnvelope(String grpId, String hostId) {

    }

    @Override
    public void OnNotifyBlacklist(String grpId, String userId) {

    }

    @Override
    public void OnNotifyForbidden(String grpId, String userId) {

    }

    @Override
    public void OnNotifySettedMgr(String grpId, String userId) {

    }
}
