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
import org.dync.teameeting.sdkmsgclient.msgs.MSMessage;
import org.dync.teameeting.sdkmsgclient.msgs.MSMessageManager;
import org.dync.teameeting.sdkmsgclient.msgs.MSSubMessage;
import org.dync.teameeting.sdkmsgclient.msgs.MSSubMessageDelegate;


/**
 * Created by hp on 7/12/16.
 */
public class MsgClientDelegateImplement implements MSClientDelegate, MSGroupDelegate, MSSubMessageDelegate {

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

        MSSubMessage.MSTxtMessage txtMsg = new MSSubMessage.MSTxtMessage();
        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        txtMsg.setContent(cont);
        txtMsg.setGroupId(groupid);
        txtMsg.setPush(0);
        String outmsgid = mMsMessageManager.sendTxtMsg(txtMsg);
        System.out.println("SendMsgTo sendTxtMsg get msgid:"+outmsgid);
    }

    public void SendMsgTos() {
        System.out.println("SendMsgTos was called...");

        MSSubMessage.MSTxtMessage txtMsg = new MSSubMessage.MSTxtMessage();
        String cont = "hahaha";
        String groupid = "wocaowocaowocao";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        txtMsg.setContent(cont);
        txtMsg.setGroupId(groupid);
        txtMsg.setPush(0);
        String outmsgid = mMsMessageManager.sendTxtMsgTos(txtMsg, arr);
        System.out.println("SendMsgTos sendTxtMsgTos get msgid:" + outmsgid);
    }

    public void SendMsgToUser() {
        System.out.println("SendMsgToUser was called...");

        MSSubMessage.MSTxtMessage txtMsg = new MSSubMessage.MSTxtMessage();
        String cont = "hahaha";
        String touser = "BCD9D958-985A-4454-B2C8-1551DB9C1A8A";
        txtMsg.setContent(cont);
        txtMsg.setToId(touser);
        txtMsg.setPush(0);
        String outmsgid = mMsMessageManager.sendTxtMsgToUser(txtMsg);
        System.out.println("SendMsgToUser sendTxtMsgToUser get msgid:" + outmsgid);
    }

    public void SendMsgToUsers() {
        System.out.println("SendMsgToUsers was called...");

        MSSubMessage.MSTxtMessage txtMsg = new MSSubMessage.MSTxtMessage();
        String cont = "hahaha";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        String[] arr = {touser1, touser2};
        txtMsg.setContent(cont);
        String outmsgid = mMsMessageManager.sendTxtMsgToUsers(txtMsg, arr);
        System.out.println("SendMsgToUsers sendTxtMsgToUsers get msgid:" + outmsgid);
    }


    public void NotifyMsgLive() {
        System.out.println("NotifyMsgLive was called...");

        MSSubMessage.MSLivMessage livMsg = new MSSubMessage.MSLivMessage();
        String groupid = "wocaowocaowocao";
        String touser = "dcd54ddf2ee1df11";
        livMsg.setGroupId(groupid);
        livMsg.setToId(touser);
        livMsg.setFlag(1);
        livMsg.setPush(1);
        livMsg.setToNickName("to测试NickName测试");
        String outmsgid = mMsMessageManager.sendNotifyLive(livMsg);
        System.out.println("NotifyMsgLive sendNotifyLive get msgid:"+outmsgid);
    }

    public void NotifyMsgRedEnvelope() {
        System.out.println("NotifyMsgRedEnvelope was called...");

        MSSubMessage.MSRenMessage renMsg = new MSSubMessage.MSRenMessage();
        String groupid = "wocaowocaowocao";
        String touser = "dcd54ddf2ee1df11";
        String cash = "123.45";
        String wishcont = "happy to you";
        renMsg.setGroupId(groupid);
        renMsg.setToId(touser);
        renMsg.setCash(cash);
        renMsg.setWishcont(wishcont);
        renMsg.setPush(1);
        renMsg.setToNickName("to测试NickName测试");
        String outmsgid = mMsMessageManager.sendNotifyRedEnvelope(renMsg);
        System.out.println("NotifyMsgRedEnvelope sendNotifyRedEnvelope get msgid:"+outmsgid);
    }

    public void NotifyMsgBlacklist() {
        System.out.println("NotifyMsgBlacklist was called...");

        MSSubMessage.MSBlkMessage blkMsg = new MSSubMessage.MSBlkMessage();
        String groupid = "wocaowocaowocao";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "BCD9D958-985A-4454-B2C8-1551DB9C1A8A";
        String[] arr = {touser1, touser2};
        blkMsg.setGroupId(groupid);
        blkMsg.setToId(touser1);
        blkMsg.setFlag(1);
        blkMsg.setPush(1);
        blkMsg.setToNickName("to测试NickName测试");
        String outmsgid = mMsMessageManager.sendNotifyBlacklist(blkMsg, arr);
        System.out.println("NotifyMsgBlacklist sendNotifyBlacklist get msgid:"+outmsgid);
    }

    public void NotifyMsgForbidden() {
        System.out.println("NotifyMsgForbidden was called...");

        MSSubMessage.MSFbdMessage fbdMsg = new MSSubMessage.MSFbdMessage();
        String groupid = "wocaowocaowocao";
        String touser1 = "dcd54ddf2ee1df11";
        String touser2 = "dcd54ddf2ee1df11";
        //String[] arr = {touser1, touser2};
        fbdMsg.setGroupId(groupid);
        fbdMsg.setToId(touser1);
        fbdMsg.setFlag(1);
        fbdMsg.setPush(1);
        fbdMsg.setToNickName("to测试NickName测试");
        String outmsgid = mMsMessageManager.sendNotifyForbidden(fbdMsg, null);
        System.out.println("NotifyMsgForbidden sendNotifyForbidden get msgid:"+outmsgid);
    }

    public void NotifyMsgSettedMgr() {
        System.out.println("NotifyMsgSettedMgr was called...");

        MSSubMessage.MSMgrMessage mgrMsg = new MSSubMessage.MSMgrMessage();
        String groupid = "wocaowocaowocao";
        String touser1 = "dcd54ddf2ee1df11";
        mgrMsg.setGroupId(groupid);
        mgrMsg.setToId(touser1);
        mgrMsg.setFlag(1);
        mgrMsg.setPush(1);
        mgrMsg.setToNickName("to测试NickName测试");
        String outmsgid = mMsMessageManager.sendNotifySettedMgr(mgrMsg);
        System.out.println("NotifyMsgSettedMgr sendNotifySettedMgr get msgid:"+outmsgid);
    }

    public void EnablePush() {
        System.out.println("EnablePush  mMsClientManager.enable push was called");
        mMsClientManager.enablePush(2);
    }

    public void MuteNotify() {
        System.out.println("MuteNotify  mMsClientManager.mute notify was called");
        mMsClientManager.muteNotify(2);
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
    public void OnRecvTxtMessage(MSMessage txtMsg) {
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
    public void OnRecvSelfDefMessage(MSMessage sdefMsg) {
        String s = "OnRecvSelfDefMessage msg:" + sdefMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyLive(MSMessage livMsg) {
        String s = "OnNotifyLive msg:" + livMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyRedEnvelope(MSMessage renMsg) {
        String s = "OnNotifyRedEnvelope msg:" + renMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifyBlacklist(MSMessage blkMsg) {
        String s = "OnNotifyBlacklist msg:" + blkMsg;

        System.out.println(s);
    }

    @Override
    public void OnNotifyForbidden(MSMessage fbdMsg) {
        String s = "OnNotifyForbidden msg:" + fbdMsg;
        System.out.println(s);
    }

    @Override
    public void OnNotifySettedMgr(MSMessage fbdMsg) {
        String s = "OnNotifySettedMgrg msg:" + fbdMsg;
        System.out.println(s);
    }
}
