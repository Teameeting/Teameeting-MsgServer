package org.dync.teameeting.sdkmsgclientandroid;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
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

        mStrUserId = Settings.Secure.getString(mContext.getContentResolver(), Settings.Secure.ANDROID_ID);
        mStrToken = "hihihihi";
        mStrNname = "dddddddd";

        mMsClientManager = new MSClientManager();
        mMsClientManager.initMsgClient(mContext, mStrUserId, mStrToken, mStrNname);
        mMsGroupManager = new MSGroupManager();
        mMsMessageManager = new MSMessageManager();

        mMsClientManager.addDelegate(this);
        mMsGroupManager.addDelegate(this);
        mMsMessageManager.addDelegate(this);

        if (0==mMsClientManager.connToServer(mServer, mPort)) {
            System.out.println("MsgClientManager connToServer invoke ok");
        } else {
            System.err.println("MsgClientManager connToServer invoke failed...!!!!!!!");
        }
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
        //String touser = "8ca64d158a505876";
        String touser = "dcd54ddf2ee1df11";
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
        String s = "OnMsgServerConnectingd was called";
        System.out.println(s);
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
        String s = "OnMsgClientInitializing was called";
        System.out.println(s);
    }

    @Override
    public void OnMsgClientInitialized() {
        String s = "OnMsgClientInitialized was called";
        System.out.println(s);
    }

    @Override
    public void OnMsgClientUnInitialize() {
        String s = "OnMsgClientUnInitialize was called";
        System.out.println(s);
    }

    @Override
    public void OnMsgClientInitializeFailure() {
        String s = "OnMsgClientInitializeFailure was called";
        System.out.println(s);
    }

    @Override
    public void OnAddGroupSuccess(String grpId) {
        String s = "OnAddGroupSuccess was called grpId:"+grpId;
        System.out.println(s);
    }

    @Override
    public void OnAddGroupFailed(String grpId, String reason, int code) {
        String s = "OnAddGroupFailed was called, grpId:"+grpId+", reason:"+reason+", code:"+code;
        System.out.println(s);
    }

    @Override
    public void OnRmvGroupSuccess(String grpId) {
        String s = "OnRmvGroupSuccess was called, grpId:"+grpId;
        System.out.println(s);
    }

    @Override
    public void OnRmvGroupFailed(String grpId, String reason, int code) {
        String s = "OnRmvGroupFailed was called, grpId:"+grpId+", reason:"+reason+", code:"+code;
        System.out.println(s);
    }

    @Override
    public void OnSendMessage(String msgId, int code) {
        String s = "OnSendMessage was called, msgId:"+msgId+", code:"+code;
        System.out.println(s);
    }

    @Override
    public void OnRecvTxtMessage(MSTxtMessage txtMsg) {
        String s = "OnRecvTxtMessage msg:" + txtMsg;
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
        String s = "OnRecvSelfDefMessage msg:" + sdefMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyLive(MSTxtMessage txtMsg) {
        String s = "OnNotifyLive msg:" + txtMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyRedEnvelope(MSTxtMessage txtMsg) {
        String s = "OnNotifyRedEnvelope msg:" + txtMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyBlacklist(MSTxtMessage txtMsg) {
        String s = "OnNotifyBlacklist msg:" + txtMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyForbidden(MSTxtMessage txtMsg) {
        String s = "OnNotifyForbidden msg:" + txtMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifySettedMgr(MSTxtMessage txtMsg) {
        String s = "OnNotifySettedMgrg msg:" + txtMsg;
        System.out.println(s);
    }
}
