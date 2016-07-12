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

    public boolean IsUserExists(String userId) {
        return mSqlite3Db.IsUserExistsInDb(userId);
    }

    public void AddUser(String userId) {
        mSqlite3Db.InsertSeqn(userId, 0);
    }

    public void UpdateUserSeqn(String userId, long seqn) {
        mSqlite3Db.UpdateSeqn(userId, seqn);
    }

    public long GetUserSeqn(String userId) {
        return mSqlite3Db.SelectSeqn(userId);
    }


    public void AddGroup(String groupId) {
        mSqlite3Db.InsertGroupId(groupId);
    }

    public void AddGroupSeqn(String grpId, long seqn) {
        mSqlite3Db.InsertSeqn(grpId, seqn);
    }

    public void UpdateGroupSeqn(String grpId, long seqn) {
        mSqlite3Db.UpdateSeqn(grpId, seqn);
    }

    public void UpdateGroupInfo(String grpId, long seqn, int isFetched) {
        mSqlite3Db.UpdateSeqn(grpId, seqn, isFetched);
    }

    public long GetGroupSeqn(String grpId) {
        return mSqlite3Db.SelectSeqn(grpId);
    }

    public ArrayList<GroupInfo> GetGroupInfo() {
        return mSqlite3Db.SelectGroupInfo();
    }

    public void DelGroup(String groupId) {
        mSqlite3Db.DeleteGroupId(groupId);
    }
}
