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
    public void addDelegate(MSTxtMessageDelegate txtDelegate) {
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
    public void delDelegate(MSTxtMessageDelegate txtDelegate) {
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
     *  you send message in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      content: the message you send
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or content is null
     */
    public String sendTxtMsg(String grpId, String content) {
        return MsgClient.getInstance().MCSendTxtMsg(grpId, content);
    }

    /**
     *  you send content to usres in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      users: the users you will send to in grpId
     *      content: the message you send
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or users or content is null
     */
    public String sendTxtMsgTos(String grpId, String[] userIds, String content) {
        return MsgClient.getInstance().MCSendTxtMsgTos(grpId, userIds, content);
    }

    /**
     *  you send content to userId
     *
     *  params:
     *      usrId: the user id you will send to
     *      content: the message you send
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: usrId or content is null
     */
    public String sendTxtMsgToUser(String usrId, String content) {
        return MsgClient.getInstance().MCSendTxtMsgToUsr(usrId, content);
    }

    /**
     *  you send content to userIds
     *
     *  params:
     *      usrIds: the users ids you will send to
     *      content: the message you send
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: usrIds or content is null
     */
    public String sendTxtMsgToUsers(String[] usrIds, String content) {
        return MsgClient.getInstance().MCSendTxtMsgToUsrs(usrIds, content);
    }

    /**
     *  hostId begin to live in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      hostId: the id of the host who is living, here means yourself id
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or hostId is null
     */
    public String sendNotifyLive(String grpId, String hostId) {
        return MsgClient.getInstance().MCNotifyLive(grpId, hostId);
    }

    /**
     *  you send red-envelope to hostId in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      hostId: the id of the host you will send to
     *      cash: the cash mount you will send to host
     *      cont: the wish message you want say to host
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or hostId or cash or cont is null
     */
    public String sendNotifyRedEnvelope(String grpId, String hostId, String cash, String cont) {
        return MsgClient.getInstance().MCNotifyRedEnvelope(grpId, hostId, cash, cont);
    }

    /**
     *  userId was push to blacklist in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      userId: the user you want to pull blacklist
     *      notifys: the users you want to notify, want them to know, usually group managers or group owner
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or userId or notifys is null
     */
    public String sendNotifyBlacklist(String grpId, String userId, String[] notifys) {
        return MsgClient.getInstance().MCNotifyBlacklist(grpId, userId, notifys);
    }

    /**
     *  userId was forbidden in grpId
     *
     *  params:
     *      grpId: the group id you are in
     *      userId: the user you want to forbidden talk in this group
     *      notifys: the users you want to notify, want them to know, usually group managers or group owner
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or userId or notifys is null
     */
    public String sendNotifyForbidden(String grpId, String userId, String[] notifys) {
        return MsgClient.getInstance().MCNotifyForbidden(grpId, userId, notifys);
    }

    /**
     *  userId in grpId was setted to be manager, this message will notify all members
     *
     *  params:
     *      grpId: the group id you are in
     *      userId: the user you want to set to be manager
     *      cmsgid: the msgid of the message you send
     *      after invoke, you will get the id of this message
     *
     *  return:
     *      >0: the whole msg length
     *      -100: cmsgid is null
     *      -101: grpId or userId or notifys is null
     */
    public String sendNotifySettedMgr(String grpId, String userId) {
        String[] notifys = {};
        return MsgClient.getInstance().MCNotifySettedMgr(grpId, userId, notifys);
    }
}
