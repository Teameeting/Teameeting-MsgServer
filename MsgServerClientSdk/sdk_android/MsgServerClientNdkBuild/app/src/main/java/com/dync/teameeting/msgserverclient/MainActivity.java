package com.dync.teameeting.msgserverclient;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.dync.teameeting.msgserverclient.msgs.TMMsgSender;

public class MainActivity extends ActionBarActivity implements View.OnClickListener{

    public TMMsgSender            mMsgSender;
    public final String           mServer = "192.168.7.39";
    public final int              mPort = 9210;
    public final String           mUserid = "test001";
    public final String           mPass = "123456";
    public final String           mRoomid = "roomid";
    public final String           mMsg = "hello world";

    public Button                 mBtnLogin   = null;
    public Button                 mBtnCreate  = null;
    public Button                 mBtnEnter   = null;
    public Button                 mBtnSndMsg  = null;
    public Button                 mBtnLeave   = null;
    public Button                 mBtnDestroy = null;
    public Button                 mBtnLogout  = null;
    public TextView               mTxtView    = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mBtnLogin = (Button)findViewById(R.id.btnlogin);
        mBtnLogin.setOnClickListener(this);

        mBtnCreate = (Button)findViewById(R.id.btncreate);
        mBtnCreate.setOnClickListener(this);

        mBtnEnter = (Button)findViewById(R.id.btnenter);
        mBtnEnter.setOnClickListener(this);

        mBtnSndMsg = (Button)findViewById(R.id.btnsndmsg);
        mBtnSndMsg.setOnClickListener(this);

        mBtnLeave = (Button)findViewById(R.id.btnleave);
        mBtnLeave.setOnClickListener(this);

        mBtnDestroy = (Button)findViewById(R.id.btndestroy);
        mBtnDestroy.setOnClickListener(this);

        mBtnLogout = (Button)findViewById(R.id.btnlogout);
        mBtnLogout.setOnClickListener(this);

        mTxtView = (TextView)findViewById(R.id.textViewResult);

        mMsgSender = new TMMsgSender(this, mTxtView);
        System.out.println("MainActivity call mMsgSender.TMInit...");
        mMsgSender.TMInit(mServer, mPort);
        try {
            Thread.sleep(1);
        } catch  (Exception e) {

        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        System.out.println("MainActivity call mMsgSender.TMLogout...");
        mMsgSender.TMLogout(mUserid, mPass);
        try {
            Thread.sleep(1);
        } catch  (Exception e) {

        }
        System.out.println("MainActivity call mMsgSender.TMUnin...");
        mMsgSender.TMUnin();
        try {
            Thread.sleep(1);
        } catch  (Exception e) {

        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnlogin:
            {
                System.out.println("MainActivity call mMsgSender.TMLogin...");
                mMsgSender.TMLogin(mUserid, mPass);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btncreate:
            {
                System.out.println("MainActivity call mMsgSender.TMCreateRoom...");
                mMsgSender.TMCreateRoom(mUserid, mPass, mRoomid);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btnenter:
            {
                System.out.println("MainActivity call mMsgSender.TMEnterRoom...");
                mMsgSender.TMEnterRoom(mUserid, mPass, mRoomid);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btnsndmsg:
            {
                System.out.println("MainActivity call mMsgSender.TMSndMsg...");
                mMsgSender.TMSndMsg(mUserid, mPass, mRoomid, mMsg);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btnleave:
            {
                System.out.println("MainActivity call mMsgSender.TMLeaveRoom...");
                mMsgSender.TMLeaveRoom(mUserid, mPass, mRoomid);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btndestroy:
            {
                System.out.println("MainActivity call mMsgSender.TMDestroyRoom...");
                mMsgSender.TMDestroyRoom(mUserid, mPass, mRoomid);
                try {
                    Thread.sleep(1);
                } catch  (Exception e) {

                }
            }
                break;
            case R.id.btnlogout:
            {
                System.out.println("MainActivity call mMsgSender.TMLogout..");
                mMsgSender.TMLogout(mUserid, mPass);
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
