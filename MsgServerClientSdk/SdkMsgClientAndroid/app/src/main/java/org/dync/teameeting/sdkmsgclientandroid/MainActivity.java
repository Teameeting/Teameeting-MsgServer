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

import org.dync.teameeting.sdkmsgclient.MeetMsgType;
import org.dync.teameeting.sdkmsgclient.MeetingMsg;
import org.dync.teameeting.sdkmsgclient.MeetingMsg.MeetMsg;
import org.dync.teameeting.sdkmsgclient.jni.JMClientHelper;
import org.dync.teameeting.sdkmsgclient.jni.JMClientType;
import org.dync.teameeting.sdkmsgclient.msgs.TMMsgSender;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private final String          TAG = "Demo";
    public final String           mServer = "192.168.7.207";
    public final int              mPort = 6630;
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
                MeetMsg meetMsg = MeetingMsg.MeetMsg.parseFrom(msg.getBytes());
                ;
                System.out.println(meetMsg);
                    switch (meetMsg.getMsgTag().getNumber()) {
                        case MeetMsgType.EMsgTag.TCHAT_VALUE:
                            content = "EMsgTag Chat, cont: " + meetMsg.getMsgCont();
                            break;
                        case MeetMsgType.EMsgTag.TNOTIFY_VALUE:
                            content = "EMsgTag Notify, cont: " + meetMsg.getMsgCont();
                            break;
                        case MeetMsgType.EMsgTag.TENTER_VALUE:
                            content = "EMsgTag Enter, cont: " + meetMsg.getMsgCont();
                            break;
                        case MeetMsgType.EMsgTag.TLEAVE_VALUE:
                            content = "EMsgTag Leave, cont: " + meetMsg.getMsgCont();
                            break;
                        default:
                            break;
                    }
            } catch (com.google.protobuf.InvalidProtocolBufferException e) {
                System.out.println("InvalidProtocolBufferException:"+e);
            }

            if (content!=null) {
                Log.i(TAG, content);
                Message tMsg = Message.obtain();
                tMsg.what = 1;
                tMsg.obj = content;
                mHandler.sendMessage(tMsg);
            }
        }

        @Override
        public void OnGetMsg(String msg) {
            String s = "OnGetMsg msg:"+ msg;
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
        }

        @Override
        public void OnMsgServerConnected() {
            String s = "OnMsgServerConnected was called";
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
        }

        @Override
        public void OnMsgServerDisconnect() {
            String s = "OnMsgServerDisconnect was called";
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
        }

        @Override
        public void OnMsgServerConnectionFailure() {
            String s = "OnMsgServerConnectionFailure was called";
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
        }

        @Override
        public void OnMsgServerState(int connStatus) {
            String s = "OnMsgServerState was called serverState:"+connStatus;
            System.out.println(s);
            Message tMsg = Message.obtain();
            tMsg.what = 1;
            tMsg.obj = s;
            mHandler.sendMessage(tMsg);
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
        while(mMsgSender.TMConnStatus()!=JMClientType.CSCONNECTED) {
            try {
                System.out.println("MainActivity mMsgSender Try to connect to server...");
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        try {
            Thread.sleep(1000);
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
                mMsgSender.TMOptRoom(MeetMsgType.EMsgTag.TENTER, mRoomid, mRname, "");
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
                mMsgSender.TMOptRoom(MeetMsgType.EMsgTag.TLEAVE, mRoomid, mRname, "");
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotify:
            {
                System.out.println("MainActivity call mMsgSender.TMNotifyMsg...");
                mMsgSender.TMNotifyMsg(mRoomid, mRname, MeetMsgType.EMsgTag.TNOTIFY, mNotifyMsg);
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

