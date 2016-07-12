package org.dync.teameeting.sdkmsgclient.msgs;

import org.dync.teameeting.sdkmsgclient.msgs.MSTxtMessage;

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
    public void OnNotifyLive(String grpId, String hostId);

    /**
     * when hostId in grpId recv red-envelope
     * <p/>
     * params:
     */
    public void OnNotifyRedEnvelope(String grpId, String hostId);

    /**
     * when userId was push to blacklist in grpId
     * <p/>
     * params:
     */
    public void OnNotifyBlacklist(String grpId, String userId);

    /**
     * when userId was forbidden in grpId
     * <p/>
     * params:
     */
    public void OnNotifyForbidden(String grpId, String userId);

    /**
     * when userId in grpId was setted to be manager
     * <p/>
     * params:
     */
    public void OnNotifySettedMgr(String grpId, String userId);
}
