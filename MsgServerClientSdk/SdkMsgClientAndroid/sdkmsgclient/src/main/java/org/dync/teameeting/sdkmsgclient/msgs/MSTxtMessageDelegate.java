package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public interface MSTxtMessageDelegate {
    /**
     * when the msg you send reached server
     * <p/>
     * params:
     */
    public void OnSendMessage(String msgId, int code);

    /**
     * when recv text msg
     * <p/>
     * params:
     */
    public void OnRecvTxtMessage(MSTxtMessage txtMsg);

    /**
     * when recv self define msg, not used now
     * <p/>
     * params:
     */
    public void OnRecvSelfDefMessage(MSTxtMessage sdefMsg);

    /**
     * when hostId begin to live in grpId
     * <p/>
     * params:
     */
    public void OnNotifyLive(MSTxtMessage txtMsg);

    /**
     * when hostId in grpId recv red-envelope
     * <p/>
     * params:
     */
    public void OnNotifyRedEnvelope(MSTxtMessage txtMsg);

    /**
     * when userId was push to blacklist in grpId
     * <p/>
     * params:
     */
    public void OnNotifyBlacklist(MSTxtMessage txtMsg);

    /**
     * when userId was forbidden in grpId
     * <p/>
     * params:
     */
    public void OnNotifyForbidden(MSTxtMessage txtMsg);

    /**
     * when userId in grpId was setted to be manager
     * <p/>
     * params:
     */
    public void OnNotifySettedMgr(MSTxtMessage txtMsg);
}
