package org.dync.teameeting.sdkmsgclientandroid;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.dync.teameeting.sdkmsgclient.jni.JMClientHelper;
import org.dync.teameeting.sdkmsgclient.jni.JMClientType;
import org.dync.teameeting.sdkmsgclient.msgs.TMMsgSender;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private final String          TAG = "Demo";
    public final String           mServer = "192.168.7.39";
    public final int              mPort = 9210;
    public final String           mUid = "8ca64d158a505876";
    public final String           mToken = "7d6a1cafeb40616ddf0c7d490771330f98a8741ae92fc450a28b2a27ebf9156a";
    public final String           mNname = "nickname";
    public final String           mRoomid = "400000000440";
    public final String           mRname  = "roomname";
    public final String           mMsg = "hello world";
    public final String           mNotifyMsg = "Notify Notify Notify!!";

    public Button mBtnEnter   = null;
    public Button                 mBtnSndMsg  = null;
    public Button                 mBtnLeave   = null;
    public Button                 mBtnNotify  = null;
    public TextView mTxtView    = null;

    public TMMsgSender mMsgSender  =  null;
    public Context mContext    = null;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            String s = msg.obj.toString();
            System.out.println("handleMessage s:"+s);
            mTxtView.setText(s);
        }
    };

    public JMClientHelper mJMClientHelper = new JMClientHelper() {
        /**
         * implement for JMClientHelper
         * */


        @Override
        public void OnSndMsg(String msg) {
            String s = "OnSndMsg msg:" + msg;
            System.out.println(s);
            String content = null;

            try {
                JSONObject json = new JSONObject(msg);
                int    messagetype = json.getInt("messagetype");
                int    signaltype = json.getInt("signaltype");
                int    cmd = json.getInt("cmd");
                int    action = json.getInt("action");
                int    tags = json.getInt("tags");
                int    type = json.getInt("type");
                int    nmem = json.getInt("nmem");
                long ntime = json.getLong("ntime");
                long mseq = json.getLong("mseq");
                String from = json.getString("from");
                String room = json.getString("room");
                String to = json.getString("to");
                String cont = json.getString("cont");
                int code = json.getInt("code");

                Log.e(TAG, "OnSndMsg: messagetype:" + messagetype +
                        ", signaltype:" + signaltype +
                        ", cmd:" + cmd +
                        ", action:" + action +
                        ", tags:" + tags +
                        ", type:" + type +
                        ", nmem:" + nmem +
                        ", ntime:" + ntime +
                        ", mseq:" + mseq +
                        ", from:" + from +
                        ", room:" + room +
                        ", to:" + to +
                        ", cont:" + cont +
                        ", code:" + code);

                if (messagetype== JMClientType.MCMESSAGETYPE_REQUEST) {
                    switch(tags) {
                        case JMClientType.MCSENDTAGS_TALK:
                            content = "MCSENDTAGS_TALK: " + cont;
                            break;
                        case JMClientType.MCSENDTAGS_ENTER:
                            content = "MCSENDTAGS_ENTER: " + cont;
                            break;
                        case JMClientType.MCSENDTAGS_LEAVE:
                            content = "MCSENDTAGS_LEAVE: " + cont;
                            break;
                        default:
                            break;
                    }
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }

            if (content!=null) {
                Log.i(TAG, content);
                Message mmsg = Message.obtain();
                mmsg.what = 1;
                mmsg.obj = content;
                mHandler.sendMessage(mmsg);
            }
        }

        @Override
        public void OnGetMsg(String msg) {
            String s = "OnGetMsg msg:"+ msg;
            System.out.println(s);
            Message mmsg = Message.obtain();
            mmsg.what = 1;
            mmsg.obj = s;
            mHandler.sendMessage(mmsg);
        }

        @Override
        public void OnMsgServerConnected() {
            String s = "OnMsgServerConnected was called";
            System.out.println(s);
            Message mmsg = Message.obtain();
            mmsg.what = 1;
            mmsg.obj = s;
            mHandler.sendMessage(mmsg);
        }

        @Override
        public void OnMsgServerDisconnect() {
            String s = "OnMsgServerDisconnect was called";
            System.out.println(s);
            Message mmsg = Message.obtain();
            mmsg.what = 1;
            mmsg.obj = s;
            mHandler.sendMessage(mmsg);
        }

        @Override
        public void OnMsgServerConnectionFailure() {
            String s = "OnMsgServerConnectionFailure was called";
            System.out.println(s);
            Message mmsg = Message.obtain();
            mmsg.what = 1;
            mmsg.obj = s;
            mHandler.sendMessage(mmsg);
        }

        @Override
        public void OnMsgServerState(int connStatus) {
            String s = "OnMsgServerState was called serverState:"+connStatus;
            System.out.println(s);
            Message mmsg = Message.obtain();
            mmsg.what = 1;
            mmsg.obj = s;
            mHandler.sendMessage(mmsg);
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        mBtnEnter = (Button)findViewById(R.id.btnenter);
        mBtnEnter.setOnClickListener(this);

        mBtnSndMsg = (Button)findViewById(R.id.btnsndmsg);
        mBtnSndMsg.setOnClickListener(this);

        mBtnLeave = (Button)findViewById(R.id.btnleave);
        mBtnLeave.setOnClickListener(this);

        mBtnNotify = (Button)findViewById(R.id.btnnotify);
        mBtnNotify.setOnClickListener(this);

        mTxtView = (TextView)findViewById(R.id.textViewResult);

        mContext = this;
        mMsgSender = new TMMsgSender(mContext, mJMClientHelper);
        System.out.println("MainActivity call mMsgSender.TMInit...");

        mMsgSender.TMInit(mUid, mToken, mNname, mServer, mPort);
        while(mMsgSender.TMConnStatus()!=JMClientType.CSNOT_CONNECTED) {
            try {
                System.out.println("MainActivity mMsgSender Try to connect to server...");
                Thread.sleep(2);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        try {
            Thread.sleep(1);
        } catch  (Exception e) {

        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        System.out.println("MainActivity call mMsgSender.TMUnin...");
        mMsgSender.TMUnin();
        try {
            Thread.sleep(1);
        } catch  (Exception e) {

        }
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnenter:
            {
                System.out.println("MainActivity call mMsgSender.TMEnterRoom...");
                mMsgSender.TMOptRoom(JMClientType.MCCMD_ENTER, mRoomid, mRname, "");
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnsndmsg:
            {
                System.out.println("MainActivity call mMsgSender.TMSndMsg...");
                mMsgSender.TMSndMsg(mRoomid, mRname, mMsg);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnleave:
            {
                System.out.println("MainActivity call mMsgSender.TMLeaveRoom...");
                mMsgSender.TMOptRoom(JMClientType.MCCMD_LEAVE, mRoomid, mRname, "");
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotify:
            {
                System.out.println("MainActivity call mMsgSender.TMNotifyMsg...");
                mMsgSender.TMNotifyMsg(mRoomid, mRname, JMClientType.MCSENDTAGS_TALK, mNotifyMsg);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            default:
                System.out.println("Unhandled id");
                break;
        }
    }
}

