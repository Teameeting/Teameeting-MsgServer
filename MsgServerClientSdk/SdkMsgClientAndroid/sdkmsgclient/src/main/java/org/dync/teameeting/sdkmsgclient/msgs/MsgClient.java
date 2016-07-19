package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

import com.google.protobuf.InvalidProtocolBufferException;

import org.dync.teameeting.sdkmsgclient.jni.JMClientApp;
import org.dync.teameeting.sdkmsgclient.jni.JMClientHelper;
import org.dync.teameeting.sdkmsgclient.jni.JMSCbData;
import org.dync.teameeting.sdkmsgclient.jni.NativeContextRegistry;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.locks.ReentrantLock;

import pms.CommonMsg;
import pms.EntityMsg;
import pms.EntityMsgType;

/**
 * Created by hp on 7/10/16.
 */
public class MsgClient implements JMClientHelper{

    private boolean                     mIsFetched = false;
    private String                      mStrUserId = null;
    private String                      mStrToken = null;
    private String                      mStrNname = null;
    private String                      mStrUicon = null;
    private HashMap<String, Long>       mGroupSeqn = null;
    private ArrayList<GroupInfo>        mGroupInfo = null;

    private NativeContextRegistry       mNativeContext = null;
    private JMClientApp                 mMApp = null;
    private Context                     mContext = null;
    private MSClientDelegate            mClientDelegate = null;
    private MSGroupDelegate             mGroupDelegate = null;
    private MSTxtMessageDelegate        mTxtMsgDelegate = null;

    private MSSqlite3Manager            mSqlite3Manager = null;
    private ReentrantLock               mReentrantLock = null;

    private static MsgClient ourInstance = new MsgClient();

    public static MsgClient getInstance() {
        return ourInstance;
    }

    private MsgClient() {
    }

    public String getmStrUserId() {
        return mStrUserId;
    }

    public void setmStrUserId(String mStrUserId) {
        this.mStrUserId = mStrUserId;
    }

    public String getmStrToken() {
        return mStrToken;
    }

    public void setmStrToken(String mStrToken) {
        this.mStrToken = mStrToken;
    }

    public String getmStrNname() {
        return mStrNname;
    }

    public void setmStrNname(String mStrNname) {
        this.mStrNname = mStrNname;
    }

    public String getmStrUicon() {
        return mStrUicon;
    }

    public void setmStrUicon(String mStrUicon) {
        this.mStrUicon = mStrUicon;
    }

    ///////////////////////////////////////////////////////////////////
    ////////////////function for invoke////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    public int MCInit(Context context, String strUid, String strToken, String strNname) {
        assert(null == mMApp);
        mContext = context;
        mNativeContext = new NativeContextRegistry();
        mNativeContext.register(mContext);
        mMApp = new JMClientApp(getInstance());
        assert (null != mMApp);
        mStrUserId = strUid;
        mStrToken = strToken;
        mStrNname = strNname;

        mGroupSeqn = new HashMap<String, Long>();
        mGroupInfo = new ArrayList<GroupInfo>();
        mReentrantLock = new ReentrantLock();
        mSqlite3Manager = new MSSqlite3Manager();
        mSqlite3Manager.InitManager(context);
        CheckUserOrInit(mStrUserId);
        GetLocalSeqnsFromDb();
        return mMApp.Init(strUid, strToken, strNname, CommonMsg.EModuleType.TLIVE_VALUE);
    }

    public int MCUnin() {
        if (null != mSqlite3Manager) {
            mSqlite3Manager.UninManager();
        }
        if (null != mMApp) {
            return mMApp.Unin();
        } else {
            return -10;
        }
    }

    public void MCSetTxtMsgDelegate(MSTxtMessageDelegate txtMsgDelegate) {
        mTxtMsgDelegate = txtMsgDelegate;
    }

    public void MCSetGroupDelegate(MSGroupDelegate groupDelegate) {
        mGroupDelegate = groupDelegate;
    }

    public void MCSetClientDelegate(MSClientDelegate clientDelegate) {
        mClientDelegate = clientDelegate;
    }

    public int MCRegisterMsgCb() {
        if (null != mMApp) {
            return mMApp.RegisterMsgCb();
        } else {
            return -10;
        }
    }

    public int MCUnRegisterMsgCb() {
        if (null != mMApp) {
            return mMApp.UnRegisterMsgCb();
        } else {
            return -10;
        }
    }

    public int MCConnToServer(String strServer, int port) {
        if (null != mMApp) {
            mMApp.ConnToServer(strServer, port);
            return 0;
        } else {
            return -10;
        }

    }

    public int MCAddGroup(String strGroupId) {
        if (null != mMApp) {
            mMApp.AddGroup(strGroupId);
            return 0;
        } else {
            return -10;
        }

    }

    public int MCRmvGroup(String strGroupId) {
        if (null != mMApp) {
            mMApp.RmvGroup(strGroupId);
            return 0;
        } else {
            return -10;
        }

    }

    public int MCSyncSeqn() {
        if (null != mMApp) {
            if (mIsFetched)
            {

                for (String k  : mGroupSeqn.keySet())
                {
                    if (mStrUserId.compareTo(k)==0)
                    {
                        mMApp.SyncGroupSeqn(k, mGroupSeqn.get(k), CommonMsg.EMsgRole.RSENDER_VALUE);
                    } else {
                        mMApp.SyncSeqn(mGroupSeqn.get(k), CommonMsg.EMsgRole.RSENDER_VALUE);
                    }
                }
            } else {
                System.out.println("MCSyncSeqn should be fetched before called");
                this.FetchAllSeqns();
            }
            return 0;
        } else {
            return -10;
        }

    }

    public int MCSyncMsg() {
        if (null != mMApp) {
            if (mIsFetched)
            {
                for (String k : mGroupSeqn.keySet()) {
                    if (mStrUserId.compareTo(k)==0)
                    {
                        mMApp.SyncGroupData(k, mGroupSeqn.get(k));
                    } else {
                        mMApp.SyncData(mGroupSeqn.get(k));
                    }
                }
            } else {
                System.out.println("MCSyncMsg should be fetched before called");
                this.FetchAllSeqns();
            }
            return 0;
        } else {
            return -10;
        }

    }

    public String MCSendTxtMsg(String strGroupId, String strContent) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndCont(strGroupId, strContent);
        if (null != mMApp && null != jsonMsg) {
            outMsgId = mMApp.SndMsg(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TTXT_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FGROUP_VALUE);
        }
        return outMsgId;
    }

    public String MCSendTxtMsgToUsr(String strUserId, String strContent) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithUidAndCont(strUserId, strContent);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strUserId};
            outMsgId = mMApp.SndMsgTo("groupid", "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TTXT_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;

    }

    public String MCSendTxtMsgToUsrs(String[] arrUsers, String strContent) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithUidsAndCont(arrUsers, strContent);
        if (null != mMApp && null != jsonMsg) {
            outMsgId =  mMApp.SndMsgTo("groupid", "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TTXT_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUsers, arrUsers.length);
        }
        return outMsgId;
    }

    public String MCNotifyLive(String strGroupId, String strHostId) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndUid(strGroupId, strHostId);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strHostId};
            outMsgId = mMApp.SndMsgTo(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TLIV_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;
    }

    public String MCNotifyRedEnvelope(String strGroupId, String strHostId) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndUid(strGroupId, strHostId);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strHostId};
            outMsgId =  mMApp.SndMsgTo(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TREN_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;
    }

    public String MCNotifyBlacklist(String strGroupId, String strUserId) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndUid(strGroupId, strUserId);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strUserId};
            outMsgId =  mMApp.SndMsgTo(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TBLK_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;
    }

    public String MCNotifyForbidden(String strGroupId, String strUserId) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndUid(strGroupId, strUserId);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strUserId};
            outMsgId = mMApp.SndMsgTo(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TFBD_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;
    }

    public String MCNotifySettedMgr(String strGroupId, String strUserId) {
        String outMsgId = null;
        String jsonMsg = MsMsgUtil.Encode2JsonWithGrpAndUid(strGroupId, strUserId);
        if (null != mMApp && null != jsonMsg) {
            String[] arrUser = {strUserId};
            outMsgId = mMApp.SndMsgTo(strGroupId, "grpname", jsonMsg, EntityMsgType.EMsgTag.TCHAT_VALUE, EntityMsgType.EMsgType.TMGR_VALUE, CommonMsg.EModuleType.TLIVE_VALUE, CommonMsg.EMsgFlag.FSINGLE_VALUE, arrUser, 1);
        }
        return outMsgId;
    }

    public int MCConnStatus() {
        if (null != mMApp) {
            return mMApp.ConnStatus();
        } else {
            return -10;
        }
    }
    public void MCSetNickName(String strNickName) {
        if (null != mMApp) {
            setmStrNname(strNickName);
            mMApp.SetNickName(strNickName);
        }
    }

    public void MCSetUIconUrl(String strUiconUrl) {
        if (null != mMApp) {
            setmStrUicon(strUiconUrl);
            mMApp.SetUIconUrl(strUiconUrl);
        }
    }


    //////////////////////////////////////////////////////////////
    //////////////////private method//////////////////////////////
    //////////////////////////////////////////////////////////////

    private void ProcessFetchResult() {
        if (IsFetchedAll()) {
            mIsFetched = true;
            mClientDelegate.OnMsgClientInitialized();
        } else {
            FetchAllSeqns();
        }
    }

    private boolean IsFetchedAll() {
        boolean yes = true;
        for (GroupInfo it : mGroupInfo) {
            String seqnId = it.getmSeqnId();
            int isfetched = it.getmIsFetched();
            System.out.println("IsFetchedAll seqnid:"+seqnId+", isfetched:"+isfetched);
            if (isfetched==0) {
                yes = false;
                break;
            } else {
                continue;
            }
        }
        return yes;
    }

    private void FetchAllSeqns() {
        for (GroupInfo it : mGroupInfo) {
            String seqnId = it.getmSeqnId();
            int isfetched = it.getmIsFetched();
            System.out.println("IsFetchedAll seqnid:"+seqnId+", isfetched:"+isfetched);
            if (isfetched==0) {
                if (seqnId.compareTo(mStrUserId)==0) {
                    mMApp.FetchSeqn();
                }
            } else {
                FetchGroupSeqn(seqnId);
            }
        }
    }

    private void FetchUserSeqn() {
        mMApp.FetchSeqn();
    }

    private void FetchGroupSeqn(String strGroupId) {
        mMApp.FetchGroupSeqn(strGroupId);
    }

    private void UpdateGroupInfoToDb(String strSeqnId, long seqn, int isFetched) {
        System.out.println("UpdateGroupInfoToDb seqn:"+strSeqnId+", isfetched:"+isFetched+", seqn:"+seqn);
        // update NSMutableArray
        for (int i=0;i<mGroupInfo.size();i++) {
            GroupInfo it = mGroupInfo.get(i);
            if (it.getmSeqnId().compareTo(strSeqnId)==0) {
                it.setmSeqn(seqn);
                it.setmIsFetched(isFetched);
            }
        }

        //GroupSeqnMap
        UpdateLocalSeqn(strSeqnId, seqn);
        // update Database
        mSqlite3Manager.UpdateGroupInfo(strSeqnId, seqn, isFetched);
    }

    private void CheckUserOrInit(String strUid) {
        if (!mSqlite3Manager.IsUserExists(strUid)) {
            mSqlite3Manager.AddUser(strUid);
        }
    }

    private void GetLocalSeqnsFromDb() {
        if (mSqlite3Manager!=null) {
            ArrayList<GroupInfo> arr = mSqlite3Manager.GetGroupInfo();
            for (GroupInfo in : arr) {
                String seqnId = in.getmSeqnId();
                long   seqn = in.getmSeqn();
                int    isfetched = in.getmIsFetched();
                System.out.println("get group info seqnid:"+seqnId+", isfetched:"+isfetched+", seqn:"+seqn);
                mGroupSeqn.put(seqnId, seqn);
                mGroupInfo.add(in);
            }
        }
    }

    private void PutLocalSeqnsToDb() {
        System.out.println("PubLocalSeqnsToDb was called");
        if (mSqlite3Manager!=null) {
            System.out.println("UpdateUserSeqn will be called...");
            for (String k  : mGroupSeqn.keySet())
            {
                mSqlite3Manager.UpdateGroupSeqn(k, mGroupSeqn.get(k));
            }
        }
    }

    private void UpdateLocalSeqn(String strSeqnId, long seqn) {
        mReentrantLock.lock();
        mGroupSeqn.put(strSeqnId, seqn);
        mReentrantLock.unlock();
    }

    private void RemoveLocalSeqn(String strSeqnId) {
        mReentrantLock.lock();
        mGroupSeqn.remove(strSeqnId);
        mReentrantLock.unlock();
    }

    private long GetLocalSeqnFromId(String strSeqnId) {
        long lseqn = -1;
        mReentrantLock.lock();
        if (mGroupSeqn.containsKey(strSeqnId)) {
            lseqn = mGroupSeqn.get(strSeqnId);
        }
        mReentrantLock.unlock();
        return lseqn;
    }


    //////////////////////////////////////////////////////////////
    /////////////Implement JMClientHelper/////////////////////////
    //////////////////////////////////////////////////////////////

    @Override
    public void OnSndMsg(int code, String msgid) {
        System.out.println("MsgClient::OnSndMsg msgid:"+msgid+", code:"+code);
        if (null != mTxtMsgDelegate) {
            mTxtMsgDelegate.OnSendMessage(msgid, code);
        }
    }

    @Override
    public void OnCmdCallback(int code, int cmd, String groupid, JMSCbData data) {
        System.out.println("MsgClient::OnCmdCallback cmd:"+cmd+", groupid.length:"+groupid.length()+", result:"+data.type+", data.result:" + data.result + ", seqn:"+data.seqn+", data.data is:"+data.data);
        EnumMsgClient.EMSCmd ecmd = EnumMsgClient.EMSCmd.forNumber(cmd);
        switch (ecmd)
        {
            case MSADD_GROUP:
            {
                if (code == 0)
                {
                    System.out.println("OnCmdCallback add group ok, insert groupid and seqn, toggle callback");
                    if (null != mSqlite3Manager && null != mGroupDelegate) {
                        if (false == mSqlite3Manager.IsGroupExistsInDb(groupid)) {
                            mSqlite3Manager.AddGroup(groupid);
                        }
                        if (false == mSqlite3Manager.IsUserExists(groupid)) {
                            mSqlite3Manager.AddGroupSeqn(groupid, data.seqn);
                        }
                        UpdateLocalSeqn(groupid, data.seqn);
                        mGroupDelegate.OnAddGroupSuccess(groupid);
                    }
                } else if (code == -1)
                {
                    if (null != mGroupDelegate) {
                        mGroupDelegate.OnAddGroupFailed(groupid, data.data, code);
                    }
                }
            }
            break;
            case MSRMV_GROUP:
            {
                if (code == 0)
                {
                    System.out.println("OnCmdCallback del group ok, del groupid and seqn, toggle callback");
                    if (null != mSqlite3Manager && null != mGroupDelegate) {
                        mSqlite3Manager.DelGroup(groupid);
                        RemoveLocalSeqn(groupid);
                        mGroupDelegate.OnRmvGroupSuccess(groupid);
                    }
                } else if (code == -1)
                {
                    if (null != mGroupDelegate) {
                        mGroupDelegate.OnRmvGroupFailed(groupid, data.data, code);
                    }
                }
            }
            break;
            case MSFETCH_SEQN:
            {
                if (groupid.length()==0) // for user
                {
                    if (data.result==0)
                    {
                        UpdateGroupInfoToDb(mStrUserId, data.seqn, 1);
                    } else if (data.result==-1)
                    {
                        UpdateGroupInfoToDb(mStrUserId, data.seqn, -1);
                    } else if (data.result==-2)
                    {
                        UpdateGroupInfoToDb(mStrUserId, data.seqn, -2);
                    }
                } else { // for group
                    if (data.result==0)
                    {
                        UpdateGroupInfoToDb(groupid, data.seqn, 1);
                    } else if (data.result==-1)
                    {
                        UpdateGroupInfoToDb(groupid, data.seqn, -1);
                    } else if (data.result==-2)
                    {
                        UpdateGroupInfoToDb(groupid, data.seqn, -2);
                    }
                }
            }
            break;
            default:
                break;
        }
    }

    @Override
    public void OnRecvMsg(long seqn, byte[] msg) {
        System.out.println("MsgClient::OnRecvMsg was called, seqn:" + seqn + ",msg.length:" + msg.length);
        UpdateLocalSeqn(mStrUserId, seqn);

        EntityMsg.Entity ee = null;
        try {
            ee = EntityMsg.Entity.parseFrom(msg);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        if (null == ee) {
            System.out.println("OnRecvMsg parseFrom msg error, ee is null");
            return;
        }
        if (ee.getUsrFrom().compareTo(mStrUserId)==0) {
            System.out.println("MsgClient::OnRecvMsg recv the msg you just send!!!, so return\n");
            return;
        }
        System.out.println("OnRecvMsg EntityMsg.Entity msg tag:" + ee.getMsgTag() + ", cont:" + ee.getMsgCont() + ", romid:" + ee.getRomId() + ", usr_from:" + ee.getUsrFrom() + ", msgtype:" + ee.getMsgType());
        MSTxtMessage txtMsg = MsMsgUtil.DecodeJsonToMessage(ee.getMsgCont());
        if (null == txtMsg) {
            System.err.println("OnRecvMsg Decode Json To Message msg is null, error return");
            return;
        }
        txtMsg.setMillSec(ee.getMsgTime());

        if (null == mTxtMsgDelegate) {
            System.out.println("OnRecvMsg mTxtMsgDelegate is null!!!!!!");
            return;
        }

        switch (ee.getMsgType())
        {
            case TTXT:
            {
                mTxtMsgDelegate.OnRecvTxtMessage(txtMsg);
            }
                break;
            case TFIL:
            {
            }
                break;
            case TPIC:
            {
            }
                break;
            case TAUD:
            {
            }
                break;
            case TVID:
            {
            }
                break;
            case TEMJ:
            {
            }
                break;
            case TSDF:
            {
                mTxtMsgDelegate.OnRecvSelfDefMessage(txtMsg);
            }
                break;
            case TLIV:
            {
                mTxtMsgDelegate.OnNotifyLive(txtMsg);
            }
                break;
            case TREN:
            {
                mTxtMsgDelegate.OnNotifyRedEnvelope(txtMsg);
            }
                break;
            case TBLK:
            {
                mTxtMsgDelegate.OnNotifyBlacklist(txtMsg);
            }
                break;
            case TFBD:
            {
                mTxtMsgDelegate.OnNotifyForbidden(txtMsg);
            }
                break;
            case TMGR:
            {
                mTxtMsgDelegate.OnNotifySettedMgr(txtMsg);
            }
                break;
            default:
            {
                System.out.println("MsgClient::OnRecvMsg ee.GetMsgType not handled:"+ee.getMsgType());
            }
            break;
        }
    }

    @Override
    public void OnRecvGroupMsg(long seqn, String seqnid, byte[] msg) {
        System.out.println("MsgClient::OnRecvGroupMsg was called seqn:" + seqn + ", seqnid:" + seqnid + ", msg.length:" + msg.length);
        UpdateLocalSeqn(seqnid, seqn);

        EntityMsg.Entity ee = null;
        try {
            ee = EntityMsg.Entity.parseFrom(msg);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        if (null == ee) {
            System.out.println("OnRecvGroupMsg parseFrom msg error, ee is null");
            return;
        }
        if (ee.getUsrFrom().compareTo(mStrUserId)==0) {
            System.out.println("MsgClient::OnRecvMsg recv the msg you just send!!!, so return\n");
            return;
        }
        System.out.println("EntityMsg.Entity msg tag:" + ee.getMsgTag() + ", cont:" + ee.getMsgCont() + ", romid:" + ee.getRomId() + ", usr_from:" + ee.getUsrFrom() + ", msgtype:" + ee.getMsgType());
        MSTxtMessage txtMsg = MsMsgUtil.DecodeJsonToMessage(ee.getMsgCont());
        if (null == txtMsg) {
            System.err.println("OnRecvGroupMsg Decode Json To Message msg is null, error return");
            return;
        }
        txtMsg.setMillSec(ee.getMsgTime());

        if (null == mTxtMsgDelegate) {
            System.out.println("OnRecvGroupMsg mTxtMsgDelegate is null!!!!!!");
            return;
        }
        switch (ee.getMsgType())
        {
            case TTXT:
            {
                mTxtMsgDelegate.OnRecvTxtMessage(txtMsg);
            }
                break;
            case TFIL:
            {
            }
                break;
            case TPIC:
            {
            }
                break;
            case TAUD:
            {
            }
                break;
            case TVID:
            {
            }
                break;
            case TEMJ:
            {
            }
                break;
            case TSDF:
            {
            }
                break;
            case TLIV:
            {
            }
                break;
            case TREN:
            {
            }
                break;
            case TBLK:
            {
            }
                break;
            case TFBD:
            {
            }
                break;
            case TMGR:
            {
            }
                break;
            default:
            {
                System.out.println("MsgClient::OnRecvGroupMsg ee.GetMsgType not handled:"+ee.getMsgType());
            }
            break;
        }
        //PutLocalSeqnsToDb();
    }

    @Override
    public void OnSyncSeqn(long maxseqn, int role) {
        System.out.println("MsgClient::OnSyncSeqn was caled maxseqn:"+maxseqn);
//        if (null != mSqlite3Manager) {
//            mSqlite3Manager.AddUser(mStrUserId);
//            mSqlite3Manager.UpdateUserSeqn(mStrUserId, maxseqn);
//        }

        // if it is sender, this means client send msg, and just get the newmsg's seqn
        // if the new seqn is bigger 1 than cur seqn, it is ok, just update seqn.
        // if the new seqn is bigger 2 or 3 than cur seqn, this need sync data
        // if it is recver, this means client need sync data
        long lseqn = GetLocalSeqnFromId(mStrUserId);
        assert(lseqn>=0);
        long index = maxseqn - lseqn;
        if (role == CommonMsg.EMsgRole.RSENDER_VALUE)
        {
            if (index == 1)
            {
                UpdateLocalSeqn(mStrUserId, maxseqn);
            } else if (index > 1)
            {
                if (null != mMApp)
                    mMApp.SyncData(lseqn);
            }
        }  else if (role == CommonMsg.EMsgRole.RRECVER_VALUE)
        {
            if (index >0)
            {
                if (null != mMApp)
                    mMApp.SyncData(lseqn);
            }
        }
    }

    @Override
    public void OnSyncGroupSeqn(String groupid, long maxseqn) {
        System.out.println("MsgClient::OnSyncGroupSeqn was called groupid:"+groupid+", maxseqn:"+maxseqn);
//        if (null != mSqlite3Manager) {
//            mSqlite3Manager.UpdateGroupSeqn(groupid, maxseqn);
//        }
        // this need sync data
        long lseqn = GetLocalSeqnFromId(groupid);
        System.out.println("MsgClient::OnSyncGroupSeqn ===>seqn seqn:"+lseqn);
        assert(lseqn>=0);
        if (maxseqn > lseqn) {
            if (null != mMApp) {
                mMApp.SyncGroupData(groupid, lseqn);
            }
        } else {
            System.out.println("MsgClient::OnSyncGroupSeqn lseqn:"+lseqn+", maxseqn:"+maxseqn+", you are not in this group:"+groupid);
        }
    }

    @Override
    public void OnGroupNotify(int code, String seqnid) {
        if (code==0)
        {
            long seqn = GetLocalSeqnFromId(seqnid);
            System.out.println("MsgClient::OnGroupNotify ===>seqn seqn:" + seqn);
            if (seqn>0) {
                if (null != mMApp) {
                    mMApp.SyncGroupData(seqnid, seqn);
                }
            } else {
                System.out.println("MsgClient::OnGroupNotify !!!!!seqn:"+seqn+", not find seqnid:"+seqnid+", you're not in this group");
                //assert(seqn>0);
            }
        } else {
            System.out.println("MsgClient::OnGroupNotify error code is :"+code);
        }
    }

    @Override
    public void OnNotifySeqn(int code, String seqnid) {
        if (code==0)
        {
            if (seqnid.length()>0)
            {
                System.out.println("THIS SHOULD NOT BE HAPPEND HERE, seqnid:" + seqnid);
//                long seqn = GetLocalSeqnFromId(seqnid);
//                if (seqn>0) {
//                    if (null != mMApp)
//                        mMApp.SyncGroupSeqn(seqnid, seqn, CommonMsg.EMsgRole.RSENDER_VALUE);
//                } else {
//                    assert(seqn>0);
//                }
            } else if (seqnid.length()==0) // this means userid
            {
                long lseqn = GetLocalSeqnFromId(mStrUserId);
                assert(lseqn>=0);
                if (null != mMApp)
                    mMApp.SyncSeqn(lseqn, CommonMsg.EMsgRole.RSENDER_VALUE);
            }
        } else {
            System.out.println("MsgClient::OnGroupSeqn error code is :"+code);
        }
    }

    @Override
    public void OnNotifyData(int code, String seqnid) {
        if (code==0)
        {
            if (seqnid.length()>0)
            {
                long seqn = GetLocalSeqnFromId(seqnid);
                System.out.println("MsgClient::OnNotifyData ===>seqn seqn:"+seqn);
                if (seqn>0) {
                    if (null != mMApp)
                        mMApp.SyncGroupData(seqnid, seqn);
                } else {
                    System.out.println("MsgClient::OnNotifyData seqn:"+seqn+", you're not in this group:"+seqnid);
                }
            } else if (seqnid.length()==0) // this means userid
            {
                System.out.println("MsgClient::OnNotifyData notify user to sync data!!!");
                long lseqn = GetLocalSeqnFromId(mStrUserId);
                assert(lseqn>=0);
                if (null != mMApp)
                    mMApp.SyncData(lseqn);
            }
        } else {
            System.out.println("MsgClient::OnGroupNotify error code is :" + code);
        }
    }

    @Override
    public void OnMsgServerConnected() {
        System.out.println("OnMsgServerConnected was called, fetchallseqn once");
        FetchAllSeqns();
        if (null != mClientDelegate) {
            mClientDelegate.OnMsgServerConnected();
            mClientDelegate.OnMsgClientInitializing();
        }

        // check per second until get all fetch
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            public void run() {
                System.out.println("MsgClient::OnMsgServerConnected timer was called...............");
                if (IsFetchedAll()) {
                    // stop timer here
                    mIsFetched = true;
                    if (null != mClientDelegate) {
                        mClientDelegate.OnMsgClientInitialized();
                    }
                    System.out.println("MsgClient::OnMsgServerConnected dispatch_source_cancel...ok");
                    this.cancel();
                } else {
                    FetchAllSeqns();
                }
            }
        }, 1000, 1000);
    }

    @Override
    public void OnMsgServerConnecting() {
        System.out.println("OnMsgServerConnecting was called");
        if (null != mClientDelegate) {
            mClientDelegate.OnMsgServerConnecting();
        }
    }

    @Override
    public void OnMsgServerDisconnect() {
        System.out.println("OnMsgServerDisconnect was called");
        if (null != mClientDelegate) {
            mClientDelegate.OnMsgServerDisconnect();
        }
    }

    @Override
    public void OnMsgServerConnectionFailure() {
        System.out.println("OnMsgServerConnectionFailer was called");
        if (null != mClientDelegate) {
            mClientDelegate.OnMsgServerConnectionFailure();
        }
    }
}
