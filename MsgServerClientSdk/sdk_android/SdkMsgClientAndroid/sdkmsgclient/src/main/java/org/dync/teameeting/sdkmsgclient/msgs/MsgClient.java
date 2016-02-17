package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

import org.dync.teameeting.sdkmsgclient.jni.JMClientApp;
import org.dync.teameeting.sdkmsgclient.jni.JMClientHelper;
import org.dync.teameeting.sdkmsgclient.jni.NativeContextRegistry;
import org.dync.teameeting.sdkmsgclient.util.MsgUtils;

/**
 * Created by hp on 12/24/15.
 */
public abstract class MsgClient{

    protected NativeContextRegistry mNativeContext;
    protected JMClientApp mMApp;
    protected Context               mContext;

    public MsgClient(Context context, JMClientHelper helper) {
        MsgUtils.assertIsTrue(null != context && null != helper);
        mContext = context;


        mNativeContext = new NativeContextRegistry();
        mNativeContext.register(mContext);

        mMApp = new JMClientApp(helper);
    }

    public void Destroy() {
        if (null != mMApp) {
            mMApp.Destroy();
            mMApp = null;
        }
        if (null != mNativeContext) {
            mNativeContext.unRegister();
            mNativeContext = null;
        }
    }

    protected int MCConnStatus() {
       return mMApp.ConnStatus();
    }

    protected int MCInit(String strUid, String strToken, String strNname, String strServer, int nPort) {
       if (null != mMApp) {
           return mMApp.Init(strUid, strToken, strNname, strServer, nPort);
       }  else {
           return -1;
       }
    }

    protected int MCUnin() {
        if (null != mMApp) {
            return mMApp.Unin();
        } else {
            return -1;
        }
    }

    protected int MCSndMsg(String strRoomid, String strRname, String strMsg) {
        if (null != mMApp) {
            return mMApp.SndMsg(strRoomid, strRname, strMsg);
        } else {
            return -1;
        }
    }

    protected int MCGetMsg(int cmd) {
        if (null != mMApp) {
            return mMApp.GetMsg(cmd);
        } else {
            return -1;
        }
    }

    protected int MCOptRoom(int cmd, String strRoomid, String strRname, String strRemain) {
        if (null != mMApp) {
            return mMApp.OptRoom(cmd, strRoomid, strRname, strRemain);
        } else {
            return -1;
        }
    }

    protected int MCSndMsgTo(String strRoomid, String strRname, String strMsg, String[] arrUser) {
        if (null != mMApp) {
            return mMApp.SndMsgTo(strRoomid, strRname, strMsg, arrUser);
        } else {
            return -1;
        }
    }

    protected int MCNotifyMsg(String strRoomid, String strRname, int tags, String strMsg) {
        if (null != mMApp) {
            return mMApp.NotifyMsg(strRoomid, strRname, tags, strMsg);
        } else {
            return -1;
        }
    }

    protected void MCSetNickName(String strNname) {
        if (null != mMApp) {
            mMApp.SetNickName(strNname);
        }
    }
}
