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

        mMsClientManager.setNickName("hehe");
        mMsClientManager.setToken("heheheh");
        mMsClientManager.setUIconUrl("hehehehe");
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

    public void LeaveRoom() {
        System.out.println("LeaveRoom was called...");
        String groupid = "wocaowocaowocao";
        mMsGroupManager.rmvGroup(groupid);
    }

    public void SendMsgTo() {
        System.out.println("SendMsgTo was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        String outmsgid = mMsMessageManager.sendTxtMsg(groupid, cont);
        System.out.println("SendMsgTo sendTxtMsg get msgid:"+outmsgid);
    }

    public void SendMsgTos() {
        System.out.println("SendMsgTos was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        //String touser = "8ca64d158a505876";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        String outmsgid = mMsMessageManager.sendTxtMsgTos(groupid, arr, cont);
        System.out.println("SendMsgTos sendTxtMsgTos get msgid:" + outmsgid);
    }

    public void SendMsgToUser() {
        System.out.println("SendMsgToUser was called...");

        String cont = "hahaha";
        //String touser = "8ca64d158a505876";
        String touser = "BCD9D958-985A-4454-B2C8-1551DB9C1A8A";
        String outmsgid = mMsMessageManager.sendTxtMsgToUser(touser, cont);
        System.out.println("SendMsgToUser sendTxtMsgToUser get msgid:" + outmsgid);
    }

    public void SendMsgToUsers() {
        System.out.println("SendMsgToUsers was called...");

        String cont = "hahaha";
        //String touser = "8ca64d158a505876";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        String outmsgid = mMsMessageManager.sendTxtMsgToUsers(arr, cont);
        System.out.println("SendMsgToUsers sendTxtMsgToUsers get msgid:" + outmsgid);
    }


    public void NotifyMsgLive() {
        System.out.println("NotifyMsgLive was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        int flag = 1;
        String touser = "dcd54ddf2ee1df11";
        String outmsgid = mMsMessageManager.sendNotifyLive(groupid, touser, 1);
        System.out.println("NotifyMsgLive sendNotifyLive get msgid:"+outmsgid);
    }

    public void NotifyMsgRedEnvelope() {
        System.out.println("NotifyMsgRedEnvelope was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        String touser = "dcd54ddf2ee1df11";
        String cash = "123.45";
        String wishcont = "happy to you";
        String outmsgid = mMsMessageManager.sendNotifyRedEnvelope(groupid, touser, cash, wishcont);
        System.out.println("NotifyMsgRedEnvelope sendNotifyRedEnvelope get msgid:"+outmsgid);
    }

    public void NotifyMsgBlacklist() {
        System.out.println("NotifyMsgBlacklist was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        int flag = 1;
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        String outmsgid = mMsMessageManager.sendNotifyBlacklist(groupid, touser1, flag, arr);
        System.out.println("NotifyMsgBlacklist sendNotifyBlacklist get msgid:"+outmsgid);
    }

    public void NotifyMsgForbidden() {
        System.out.println("NotifyMsgForbidden was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        int flag = 1;
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        String outmsgid = mMsMessageManager.sendNotifyForbidden(groupid, touser1, flag, arr);
        System.out.println("NotifyMsgForbidden sendNotifyForbidden get msgid:"+outmsgid);
    }

    public void NotifyMsgSettedMgr() {
        System.out.println("NotifyMsgSettedMgr was called...");

        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        int flag = 1;
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String outmsgid = mMsMessageManager.sendNotifySettedMgr(groupid, touser1, flag);
        System.out.println("NotifyMsgSettedMgr sendNotifySettedMgr get msgid:"+outmsgid);
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
    public void OnNotifyLive(MSTxtMessage livMsg) {
        String s = "OnNotifyLive msg:" + livMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyRedEnvelope(MSTxtMessage renMsg) {
        String s = "OnNotifyRedEnvelope msg:" + renMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyBlacklist(MSTxtMessage blkMsg) {
        String s = "OnNotifyBlacklist msg:" + blkMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyForbidden(MSTxtMessage fbdMsg) {
        String s = "OnNotifyForbidden msg:" + fbdMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifySettedMgr(MSTxtMessage fbdMsg) {
        String s = "OnNotifySettedMgrg msg:" + fbdMsg;
        System.out.println(s);
    }
}
