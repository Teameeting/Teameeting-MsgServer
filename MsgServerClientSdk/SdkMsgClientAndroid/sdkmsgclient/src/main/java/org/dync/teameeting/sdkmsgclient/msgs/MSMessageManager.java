package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSMessageManager {
    /**
     * add message delegate
     * <p/>
     * params:
     */
    public void addDelegate(MSTxtMessageDelegate txtDelegate) {
        MsgClient.getInstance().MCSetTxtMsgDelegate(txtDelegate);
    }

    /**
     * remove message delegate
     * <p/>
     * params:
     */
    public void delDelegate(MSTxtMessageDelegate txtDelegate) {
        MsgClient.getInstance().MCSetTxtMsgDelegate(null);
    }

    /**
     * sync message when first login or refresh
     * do not call this function in an loooooop
     * <p/>
     * params:
     */
    public void syncMsg() {
        MsgClient.getInstance().MCSyncMsg();
    }

    /**
     * you send content in grpId
     * <p/>
     * params:
     */
    public String sendTxtMsg(String grpId, String content) {
        return MsgClient.getInstance().MCSendTxtMsg(grpId, content);
    }

    /**
     * you send content to userIds in grpId
     * <p/>
     * params:
     */
    public String sendTxtMsgTos(String grpId, String[] userIds, String content) {
        return MsgClient.getInstance().MCSendTxtMsgTos(grpId, userIds, content);
    }

    /**
     * you send content to userId
     * <p/>
     * params:
     */
    public String sendTxtMsgToUser(String usrId, String content) {
        return MsgClient.getInstance().MCSendTxtMsgToUsr(usrId, content);
    }

    /**
     * you send content to userIds
     * <p/>
     * params:
     */
    public String sendTxtMsgToUsers(String[] usrIds, String content) {
        return MsgClient.getInstance().MCSendTxtMsgToUsrs(usrIds, content);
    }

    /**
     * hostId begin to live in grpId
     * <p/>
     * params:
     */
    public String sendNotifyLive(String grpId, String hostId) {
        return MsgClient.getInstance().MCNotifyLive(grpId, hostId);
    }

    /**
     * you send red-envelope to hostId in grpId
     * <p/>
     * params:
     */
    public String sendNotifyRedEnvelope(String grpId, String hostId) {
        return MsgClient.getInstance().MCNotifyRedEnvelope(grpId, hostId);
    }

    /**
     * userId was push to blacklist in grpId
     * <p/>
     * params:
     */
    public String sendNotifyBlacklist(String grpId, String userId) {
        return MsgClient.getInstance().MCNotifyBlacklist(grpId, userId);
    }

    /**
     * userId was forbidden in grpId
     * <p/>
     * params:
     */
    public String sendNotifyForbidden(String grpId, String userId) {
        return MsgClient.getInstance().MCNotifyForbidden(grpId, userId);
    }

    /**
     * userId in grpId was setted to be manager
     * <p/>
     * params:
     */
    public String sendNotifySettedMgr(String grpId, String userId) {
        return MsgClient.getInstance().MCNotifySettedMgr(grpId, userId);
    }
}
