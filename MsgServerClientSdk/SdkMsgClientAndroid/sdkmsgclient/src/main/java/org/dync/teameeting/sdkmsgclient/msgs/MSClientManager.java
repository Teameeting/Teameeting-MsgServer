package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

/**
 * Created by hp on 7/10/16.
 */
public class MSClientManager {
    /**
     * init msg client
     * <p/>
     * params:
     */
    public int initMsgClient(Context context, String usrId, String token, String nName) {
        MsgClient.getInstance().MCInit(context, usrId, token, nName);
        MsgClient.getInstance().MCRegisterMsgCb();
        return 0;
    }

    /**
     * uninit msg client
     * <p/>
     * params:
     */
    public int uninMsgClient() {
        MsgClient.getInstance().MCUnRegisterMsgCb();
        MsgClient.getInstance().MCUnin();
        return 0;
    }

    /**
     * add client delegate
     * <p/>
     * params:
     */
    public void addDelegate(MSClientDelegate cltDelegate) {
        MsgClient.getInstance().MCSetClientDelegate(cltDelegate);
    }

    /**
     * remove client delegate
     * <p/>
     * params:
     */
    public void delDelegate(MSClientDelegate cltDelegate) {
        MsgClient.getInstance().MCSetClientDelegate(null);
    }

    /**
     * connect to server
     * <p/>
     * params:
     */
    public int connToServer(String server, int port) {
        return MsgClient.getInstance().MCConnToServer(server, port);
    }

    /**
     * get connection status
     * <p/>
     * params:
     */
    public int connStatus() {
        return MsgClient.getInstance().MCConnStatus();
    }
}
