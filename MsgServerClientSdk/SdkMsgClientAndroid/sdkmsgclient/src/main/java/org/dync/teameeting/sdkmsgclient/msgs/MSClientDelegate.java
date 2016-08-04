package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public interface MSClientDelegate {
    /**
     * when server was connected this function will be called
     * <p/>
     * params:
     */
    public void OnMsgServerConnected();

    /**
     * when server was connecting this function will be called
     * <p/>
     * params:
     */
    public void OnMsgServerConnecting();

    /**
     * when server was disconnected this function will be called
     * <p/>
     * params:
     */
    public void OnMsgServerDisconnect();

    /**
     * when server connect failed this function will be called
     * <p/>
     * params:
     */
    public void OnMsgServerConnectionFailure();

    /**
     * when client initializing this function will be called
     * <p/>
     * params:
     */
    public void OnMsgClientInitializing();

    /**
     * when client initialize ok this function will be called
     * <p/>
     * params:
     */
    public void OnMsgClientInitialized();

    /**
     * when client un initialize ok this function will be called
     * <p/>
     * params:
     */
    public void OnMsgClientUnInitialize();

    /**
     * when client initialize failed this function will be called
     * <p/>
     * params:
     */
    public void OnMsgClientInitializeFailure();
}
