package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public interface MSClientDelegate {
    /**
     * when server was connected
     * <p/>
     * params:
     */
    public void OnMsgServerConnected();

    /**
     * when server was connecting
     * <p/>
     * params:
     */
    public void OnMsgServerConnecting();

    /**
     * when server was disconnected
     * <p/>
     * params:
     */
    public void OnMsgServerDisconnect();

    /**
     * when server connect failed
     * <p/>
     * params:
     */
    public void OnMsgServerConnectionFailure();

    /**
     * when client initializing
     * <p/>
     * params:
     */
    public void OnMsgClientInitializing();

    /**
     * when client initialize ok
     * <p/>
     * params:
     */
    public void OnMsgClientInitialized();

    /**
     * when client un initialize
     * <p/>
     * params:
     */
    public void OnMsgClientUnInitialize();

    /**
     * when client initialize failed
     * <p/>
     * params:
     */
    public void OnMsgClientInitializeFailure();
}
