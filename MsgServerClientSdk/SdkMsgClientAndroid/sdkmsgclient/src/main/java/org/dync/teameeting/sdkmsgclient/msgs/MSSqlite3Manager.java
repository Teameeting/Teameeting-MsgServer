package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

import java.util.ArrayList;

/**
 * Created by hp on 7/11/16.
 */
public class MSSqlite3Manager {
    private static MSSqlite mSqlite3Db = null;

    public void InitManager(Context context) {
        mSqlite3Db = new MSSqlite(context);
    }

    public void UninManager() {

    }

    public boolean IsUserExists(String userId, String seqnId) {
        return mSqlite3Db.IsUserExistsInDb(userId, seqnId);
    }

    public void AddUser(String userId) {
        mSqlite3Db.InsertSeqn(userId, userId, 0);
    }

    public void UpdateUserSeqn(String userId, long seqn) {
        mSqlite3Db.UpdateSeqn(userId, userId, seqn);
    }

    public long GetUserSeqn(String userId) {
        return mSqlite3Db.SelectSeqn(userId, userId);
    }


    public void AddGroup(String userId, String groupId) {
        mSqlite3Db.InsertGroupId(userId, groupId);
    }

    public boolean IsGroupExistsInDb(String userId, String groupId) { return mSqlite3Db.IsGroupExistsInDb(userId, groupId); }

    public void AddGroupSeqn(String userId, String grpId, long seqn) {
        mSqlite3Db.InsertSeqn(userId, grpId, seqn);
    }

    public void UpdateGroupSeqn(String userId, String grpId, long seqn) {
        mSqlite3Db.UpdateSeqn(userId, grpId, seqn);
    }

    public void UpdateGroupInfo(String userId, String grpId, long seqn, int isFetched) {
        mSqlite3Db.UpdateSeqn(userId, grpId, seqn, isFetched);
    }

    public long GetGroupSeqn(String userId, String grpId) {
        return mSqlite3Db.SelectSeqn(userId, grpId);
    }

    public ArrayList<GroupInfo> GetGroupInfo(String userId) {
        return mSqlite3Db.SelectGroupInfo(userId);
    }

    public void DelGroup(String userId, String groupId) {
        mSqlite3Db.DeleteGroupId(userId, groupId);
    }
}
