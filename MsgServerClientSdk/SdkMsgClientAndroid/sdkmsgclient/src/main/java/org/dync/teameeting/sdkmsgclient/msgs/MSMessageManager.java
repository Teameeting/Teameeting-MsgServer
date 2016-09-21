package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSMessageManager {
    /**
     *  add message delegate
     *
     *  params:
     *      txtDelegate: the message delegate for callback to set
     *
     *  return:
     *      void
     */
    public void addDelegate(MSSubMessageDelegate txtDelegate) {
        MsgClient.getInstance().MCSetTxtMsgDelegate(txtDelegate);
    }

    /**
     *  remove message delegate
     *
     *  params:
     *      txtDelegate: the message delegate to delete
     *
     *  return:
     *      void
     */
    public void delDelegate(MSSubMessageDelegate txtDelegate) {
        MsgClient.getInstance().MCSetTxtMsgDelegate(null);
    }

    /**
     *  sync message when first login or refresh
     *  do not call this function in an loooooop
     *
     *  params:
     *
     *  return:
     *      void
     */
    public void syncMsg() {
        MsgClient.getInstance().MCSyncMsg();
    }

    /**
     * this will sync msg to db
     * when app will exit or crash ot leave view
     * MUST invoke this to save data
     * <p/>
     * params:
     * <p/>
     * return:
     * void
     */
    public void sync2Db() {
        MsgClient.getInstance().MCSync2Db();
    }

    /**
     *  you send message in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and Content and Push need to be setted
     *      GroupId: the group id you are in
     *      Content: the content you send
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendTxtMsg(MSSubMessage.MSTxtMessage txtMsg) {
            if (null==txtMsg || null == txtMsg.getGroupId() || null == txtMsg.getContent()) return null;
        if (txtMsg.getGroupId().length()==0 || txtMsg.getContent().length()==0) return null;
        return MsgClient.getInstance().MCSendTxtMsg(txtMsg);
    }

    /**
     *  you send content to usres in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and Content and Push need to be setted
     *      GroupId: the group id you are in
     *      Content: the message you send
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendTxtMsgTos(MSSubMessage.MSTxtMessage txtMsg, String[] userIds) {
            if (null==txtMsg || null == txtMsg.getGroupId() || null == txtMsg.getContent() || null == userIds) return null;
        if (txtMsg.getGroupId().length()==0 || txtMsg.getContent().length()==0 || userIds.length==0) return null;
        return MsgClient.getInstance().MCSendTxtMsgTos(txtMsg, userIds);
    }

    /**
     *  you send content to userId
     *
     *  params:
     *      txtMsg: the message you will send
     *      Content and ToId and Push need to be setted
     *      Content: the message you send
     *      ToId: the user id you will send to
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendTxtMsgToUser(MSSubMessage.MSTxtMessage txtMsg) {
            if (null==txtMsg || null == txtMsg.getToId() || null == txtMsg.getContent()) return null;
        if (txtMsg.getToId().length()==0 || txtMsg.getContent().length()==0) return null;
        return MsgClient.getInstance().MCSendTxtMsgToUsr(txtMsg);
    }

    /**
     *  you send content to userIds
     *
     *  params:
     *      txtMsg: the message you will send
     *      Content and Push need to be setted
     *      Content: the message you send
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *      users: the users ids you will send to
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendTxtMsgToUsers(MSSubMessage.MSTxtMessage txtMsg, String[] usrIds) {
            if (null==txtMsg || null == txtMsg.getContent() || null == usrIds) return null;
        if (txtMsg.getContent().length()==0 || usrIds.length==0) return null;
        return MsgClient.getInstance().MCSendTxtMsgToUsrs(txtMsg, usrIds);
    }

    /**
     *  hostId begin to live in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and ToId and Flag and Push need to be setted
     *      GroupId: the group id you are in
     *      ToId: the user you want to forbidden talk in this group
     *      Flag: 1, forbidden user, 0, cancel forbidden
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendNotifyLive(MSSubMessage.MSLivMessage livMsg) {
            if (null==livMsg || null==livMsg.getGroupId()) return null;
        if (livMsg.getGroupId().length()==0) return null;
        return MsgClient.getInstance().MCNotifyLive(livMsg);
    }

    /**
     *  you send red-envelope to hostId in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and ToId and and Push and Cash and Wishcont need to be setted
     *      GroupId: the group id you are in
     *      ToId: the user you want to forbidden talk in this group
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *      Cash: the cash mount you will send to host
     *      Wishcont: the wish message you want say to host
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendNotifyRedEnvelope(MSSubMessage.MSRenMessage renMsg) {
            if (null==renMsg || null == renMsg.getGroupId() || null == renMsg.getToId() || null == renMsg.getCash() || null == renMsg.getWishcont()) return null;
        if (renMsg.getGroupId().length()==0 || renMsg.getToId().length()==0 || renMsg.getCash().length()==0 || renMsg.getWishcont().length()==0) return null;
        return MsgClient.getInstance().MCNotifyRedEnvelope(renMsg);
    }

    /**
     *  userId was push to blacklist in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and ToId and Flag and Push need to be setted
     *      GroupId: the group id you are in
     *      ToId: the user you want to forbidden talk in this group
     *      Flag: 1, forbidden user, 0, cancel forbidden
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *      notifys: the users you want to notify, want them to know, usually group managers or group owner
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendNotifyBlacklist(MSSubMessage.MSBlkMessage blkMsg, String[] notifys) {
            if (null==blkMsg || null == blkMsg.getGroupId() || null == blkMsg.getToId()) return null;
        if (blkMsg.getGroupId().length()==0 || blkMsg.getToId().length()==0) return null;
        return MsgClient.getInstance().MCNotifyBlacklist(blkMsg, notifys);
    }

    /**
     *  userId was forbidden in grpId
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and ToId and Flag and Push need to be setted
     *      GroupId: the group id you are in
     *      ToId: the user you want to forbidden talk in this group
     *      Flag: 1, forbidden user, 0, cancel forbidden
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *      notifys: the users you want to notify, want them to know, usually group managers or group owner
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendNotifyForbidden(MSSubMessage.MSFbdMessage fbdMsg, String[] notifys) {
            if (null==fbdMsg || null == fbdMsg.getGroupId() || null == fbdMsg.getToId()) return null;
        if (fbdMsg.getGroupId().length()==0 || fbdMsg.getToId().length()==0) return null;
        return MsgClient.getInstance().MCNotifyForbidden(fbdMsg, notifys);
    }

    /**
     *  userId in grpId was setted to be manager, this message will notify all members
     *
     *  params:
     *      txtMsg: the message you will send
     *      GroupId and ToId and Flag and Push need to be setted
     *      GroupId: the group id you are in
     *      ToId: the user you want to set to be manager
     *      Flag: 1, set user mgr, 0, set user not mgr
     *      Push: 1, the msg will be pushed if offline, 0, not push
     *
     *  return:
     *      msgid: the id of this msg
     */
    public String sendNotifySettedMgr(MSSubMessage.MSMgrMessage mgrMsg) {
            if (null==mgrMsg || null == mgrMsg.getGroupId() || null == mgrMsg.getToId()) return null;
        if (mgrMsg.getGroupId().length()==0 || mgrMsg.getToId().length()==0) return null;
        String[] notifys = {};
        return MsgClient.getInstance().MCNotifySettedMgr(mgrMsg, notifys);
    }
}
