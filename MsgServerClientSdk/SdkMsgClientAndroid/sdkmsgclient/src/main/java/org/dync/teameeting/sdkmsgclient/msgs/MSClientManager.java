package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

/**
 * Created by hp on 7/10/16.
 */
public class MSClientManager {
    /**
     *  init msg client
     *
     *  params:
     *  usrId: the userid
     *  token: the user token
     *  nName: the user nick name
     *
     *  return:
     *      0: init ok
     *      -101: one of params are null
     */
    public int initMsgClient(Context context, String usrId, String token, String nName) {
        System.out.println("MsgClient Android Version:v1.0.0");
        if (null == usrId || usrId.length()==0 || null == token || token.length()==0 || null == nName || nName.length()==0) return -101;
        MsgClient.getInstance().MCInit(context, usrId, token, nName);
        MsgClient.getInstance().MCRegisterMsgCb();
        return 0;
    }

    /**
     *  uninit msg client
     *
     *  params:
     *
     *  return:
     *      0: unin ok
     */
    public int uninMsgClient() {
        MsgClient.getInstance().MCUnRegisterMsgCb();
        MsgClient.getInstance().MCUnin();
        return 0;
    }

    /**
     *  add client delegate
     *
     *  params:
     *      cltDelegate: the client delegate for callback to set
     *
     *  return:
     *      void
     */
    public void addDelegate(MSClientDelegate cltDelegate) {
        MsgClient.getInstance().MCSetClientDelegate(cltDelegate);
    }

    /**
     *  remove client delegate
     *
     *  params:
     *      cltDelegate: the client delegate to delete
     *
     *  return:
     *      void
     */
    public void delDelegate(MSClientDelegate cltDelegate) {
        MsgClient.getInstance().MCSetClientDelegate(null);
    }

    /**
     *  connect to server
     *
     *  params:
     *      server: the msg server ip
     *      port: the msg server port
     *
     *  return:
     *      0: connect server ok
     *      -101: params is null
     */
    public int connToServer(String server, int port) {
        if (null == server || server.length()==0 || port <0) return -101;
        return MsgClient.getInstance().MCConnToServer(server, port);
    }

    /**
     *  get connection status
     *
     *  params:
     *
     *  return:
     *      int: the state of the connection to server
     */
    public int connStatus() {
        return MsgClient.getInstance().MCConnStatus();
    }

    /**
     *  set userid
     *
     *  params:
     *      userid: the userid to set
     *
     *  (but this func has not implement)
     *
     *  return:
     *      void
     *
     */
    public void setUserid(String strUserid) {
        //DO NOT IMPLEMENT
    }

    /**
     *  set user token
     *
     *  params:
     *      token: the user token
     *
     *  return:
     *      void
     */
    public void setToken(String strToken) {
        if (null == strToken || strToken.length()==0) return;
        MsgClient.getInstance().MCSetToken(strToken);
    }

    /**
     *  set user nick name
     *
     *  params:
     *      nName: the user nick name
     *
     *  return:
     *      void
     */
    public void setNickName(String strNname) { if (null == strNname || strNname.length()==0) return;
        MsgClient.getInstance().MCSetNickName(strNname); }

    /**
     *  set user icon url
     *
     *  params:
     *      url: the user icon url
     *
     *  return:
     *      void
     */
    public void setUIconUrl(String strUicon) { if (null == strUicon || strUicon.length()==0) return;
        MsgClient.getInstance().MCSetUIconUrl(strUicon); }

    /**
     *  set if need push
     *
     *  params:
     *      push: 1, enable push, 2, disable push
     *
     *  return:
     *      void
     */
    public void enablePush(int push) {
        MsgClient.getInstance().MCEnablePush(push);
    }

    /**
     *  if mute notifications
     *
     *  params:
     *      mute: 1, mute notification, 2, not mute notification
     *
     *  return:
     *      void
     */
    public void muteNotify(int mute) {
        MsgClient.getInstance().MCMuteNotify(mute);
    }
}
