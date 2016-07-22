package org.dync.teameeting.sdkmsgclientandroid;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

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

    public Button                 mBtnEnter   = null;
    public Button                 mBtnLeave   = null;
    public Button                 mBtnSndMsg  = null;
    public Button                 mBtnSndTosMsg  = null;
    public Button                 mBtnSndToUserMsg  = null;
    public Button                 mBtnSndToUsersMsg  = null;
    public Button                 mBtnNotifyLive  = null;
    public Button                 mBtnNotifyRedEnvelope  = null;
    public Button                 mBtnNotifyBlacklist  = null;
    public Button                 mBtnNotifyForbidden  = null;
    public Button                 mBtnNotifySettedMgr  = null;
    public TextView               mTxtView    = null;

    public MsgClientDelegateImplement       mMsgClient = null;
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        mBtnEnter = (Button)findViewById(R.id.btnenter);
        mBtnEnter.setOnClickListener(this);

        mBtnLeave = (Button)findViewById(R.id.btnleave);
        mBtnLeave.setOnClickListener(this);

        mBtnSndMsg = (Button)findViewById(R.id.btnsndmsg);
        mBtnSndMsg.setOnClickListener(this);

        mBtnSndTosMsg = (Button)findViewById(R.id.btnsndmsgtos);
        mBtnSndTosMsg.setOnClickListener(this);

        mBtnSndToUserMsg = (Button)findViewById(R.id.btnsndmsgtouser);
        mBtnSndToUserMsg.setOnClickListener(this);

        mBtnSndToUsersMsg = (Button)findViewById(R.id.btnsndmsgtousers);
        mBtnSndToUsersMsg.setOnClickListener(this);

        mBtnNotifyLive = (Button)findViewById(R.id.btnnotifylive);
        mBtnNotifyLive.setOnClickListener(this);

        mBtnNotifyRedEnvelope = (Button)findViewById(R.id.btnnotifyredenvelope);
        mBtnNotifyRedEnvelope.setOnClickListener(this);

        mBtnNotifyBlacklist = (Button)findViewById(R.id.btnnotifyblacklist);
        mBtnNotifyBlacklist.setOnClickListener(this);

        mBtnNotifyForbidden = (Button)findViewById(R.id.btnnotifyforbidden);
        mBtnNotifyForbidden.setOnClickListener(this);

        mBtnNotifySettedMgr = (Button)findViewById(R.id.btnnotifysettedmgr);
        mBtnNotifySettedMgr.setOnClickListener(this);

        mTxtView = (TextView)findViewById(R.id.textViewResult);

        mContext = this;
        mMsgClient = new MsgClientDelegateImplement(mContext, mHandler);
        System.out.println("MainActivity call mMsgClient...");

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        System.out.println("MainActivity call MsgClient.Destroy...");
        mMsgClient.MsgClientDestroy();
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
                System.out.println("MainActivity call mMsgClient.EnterRoom...");
                mMsgClient.EnterRoom();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnleave:
            {
                System.out.println("MainActivity call mMsgClient.LeaveRoom...");
                mMsgClient.LeaveRoom();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnsndmsg:
            {
                System.out.println("MainActivity call mMsgClient.SndMsgTo...");
                mMsgClient.SendMsgTo();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnsndmsgtos:
            {
                System.out.println("MainActivity call mMsgClient.SndMsgTos...");
                mMsgClient.SendMsgTos();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnsndmsgtouser:
            {
                System.out.println("MainActivity call mMsgClient.SndMsgToUser...");
                mMsgClient.SendMsgToUser();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnsndmsgtousers:
            {
                System.out.println("MainActivity call mMsgClient.SndMsgToUsers...");
                mMsgClient.SendMsgToUsers();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotifylive:
            {
                System.out.println("MainActivity call mMsgClient.NotifyMsgLive...");
                mMsgClient.NotifyMsgLive();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotifyredenvelope:
            {
                System.out.println("MainActivity call mMsgClient.NotifyMsgRedEnvelope...");
                mMsgClient.NotifyMsgRedEnvelope();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotifyblacklist:
            {
                System.out.println("MainActivity call mMsgClient.NotifyMsgBlacklist...");
                mMsgClient.NotifyMsgBlacklist();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotifyforbidden:
            {
                System.out.println("MainActivity call mMsgClient.NotifyMsgFrobidden...");
                mMsgClient.NotifyMsgForbidden();
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
            break;
            case R.id.btnnotifysettedmgr:
            {
                System.out.println("MainActivity call mMsgClient.NotifyMsgSettedMgr...");
                mMsgClient.NotifyMsgSettedMgr();
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

