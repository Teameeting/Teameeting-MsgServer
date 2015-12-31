package com.dync.teameeting.msgserverclient.msgs;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

import com.dync.teameeting.msgserverclient.MsgClient;

/**
 * Created by hp on 12/24/15.
 */
public class TMMsgSender extends MsgClient {

    private TextView      mTxtView = null;
    private Activity      mActivity = null;
    private Handler       mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            String s = msg.obj.toString();
            System.out.println("handleMessage s:"+s);
            mTxtView.setText(s);
        }
    };
    public TMMsgSender(Activity activity, TextView txtView) {
        super(activity);
        mActivity = activity;
        mTxtView = txtView;
    }

    /**
     * provide for client to imvoke
     * */

    public int TMConnStatus() {
        return this.MCConnStatus();
    }

    public int TMInit(String strServer, int nPort) {
        return this.MCInit(strServer, nPort);
    }

    public int TMUnin() {
        return this.MCUnin();
    }


    public int TMLogin(String strUserid, String strPass) {
        return this.MCLogin(strUserid, strPass);
    }

    public int TMSndMsg(String strUserid, String strPass, String strRoomid, String strMsg) {
        return this.MCSndMsg(strUserid, strPass, strRoomid, strMsg);
    }

    public int TMGetMsg(String strUserid, String strPass) {
        return this.MCGetMsg(strUserid, strPass);
    }

    public int TMLogout(String strUserid, String strPass) {
        return this.MCLogout(strUserid, strPass);
    }
    public int TMCreateRoom(String strUserid, String strPass, String strRoomid) {
        return this.MCCreateRoom(strUserid, strPass, strRoomid);
    }
    public int TMEnterRoom(String strUserid, String strPass, String strRoomid) {
        return this.MCEnterRoom(strUserid, strPass, strRoomid);
    }

    public int TMLeaveRoom(String strUserid, String strPass, String strRoomid) {
        return this.MCLeaveRoom(strUserid, strPass, strRoomid);
    }

    public int TMDestroyRoom(String strUserid, String strPass, String strRoomid) {
        return  this.MCDestroyRoom(strUserid, strPass, strRoomid);
    }

    public int TMSndMsgTo(String strUserid, String strPass, String strRoomid, String strMsg, String[] arrUser) {
        return  this.MCSndMsgTo(strUserid, strPass, strRoomid, strMsg, arrUser);
    }

    /**
     * implement for JMClientHelper
    * */


    @Override
    public void OnLogin(int code) {
        String s = "OnLogin code:"+code;
        System.out.println(s);
        Message mmsg = Message.obtain();
        mmsg.what = 1;
        mmsg.obj = s;
        mHandler.sendMessage(mmsg);
    }

    @Override
    public void OnSndMsg(String msg) {
        String s = "OnSndMsg msg:" + msg;
        System.out.println(s);
        Message mmsg = Message.obtain();
        mmsg.what = 1;
        mmsg.obj = s;
        mHandler.sendMessage(mmsg);
    }

    @Override
    public void OnGetMsg(String from, String msg) {
        String s = "OnGetMsg from:" + from + ", msg:"+ msg;
        System.out.println(s);
        Message mmsg = Message.obtain();
        mmsg.what = 1;
        mmsg.obj = s;
        mHandler.sendMessage(mmsg);
    }

    @Override
    public void OnLogout(int code) {
        String s = "OnLogout code:" + code;
        System.out.println(s);
        Message mmsg = Message.obtain();
        mmsg.what = 1;
        mmsg.obj = s;
        mHandler.sendMessage(mmsg);
    }
}
