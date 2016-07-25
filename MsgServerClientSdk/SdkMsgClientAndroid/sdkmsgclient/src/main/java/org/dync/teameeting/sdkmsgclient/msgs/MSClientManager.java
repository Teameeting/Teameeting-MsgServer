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
        System.out.println("MsgClient Android Version:v1.0.0");
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

    /**
     * set userid
     * <p/>
     * params:
     */
    public void setUserid(String strUserid) { MsgClient.getInstance().MCSetUserId(strUserid); }

    /**
     * set user token
     * <p/>
     * params:
     */
    public void setToken(String strToken) { MsgClient.getInstance().MCSetToken(strToken); }

    /**
     * set user nick name
     * <p/>
     * params:
     */
    public void setNickName(String strNname) { MsgClient.getInstance().MCSetNickName(strNname); }

    /**
     * set user uicon url
     * <p/>
     * params:
     */
    public void setUIconUrl(String strUicon) { MsgClient.getInstance().MCSetUIconUrl(strUicon); }
}
