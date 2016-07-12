package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by hp on 7/11/16.
 */
public class MSSqlite {
    private static SqliteDbHelper mSql3Db = null;

    public MSSqlite(Context context) {
        mSql3Db = new SqliteDbHelper(context);
    }

    public boolean OpenDb() {
        return true;
    }

    public boolean CloseDb() {
        return true;
    }

    public boolean IsUserExistsInDb(String userId) {
        SQLiteDatabase db = mSql3Db.getReadableDatabase();
        String sqlSelect = "select count(*) from " + SqliteDbHelper.MS_TABLE_STOREID_SEQN + " where seqnId='" + userId +"';" ;
        Cursor cursor = db.rawQuery(sqlSelect, null);
        int code = 0;
        if (cursor.moveToFirst()) {
            do {
               code = cursor.getInt(0);
            } while(cursor.moveToNext());
        }
        boolean exist = ((code==0)?false:true);
        cursor.close();
        db.close();
        return exist;
    }

    public boolean InsertSeqn(String seqnId, long seqn) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        ContentValues value = new ContentValues();
        value.put("seqnId", seqnId);
        value.put("seqn", seqn);
        value.put("isfetched", 0);
        db.insert(SqliteDbHelper.MS_TABLE_STOREID_SEQN, null, value);
        db.close();
        return true;
    }

    public boolean UpdateSeqn(String seqnId, long seqn) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        ContentValues value = new ContentValues();
        value.put("seqn", seqn);
        db.update(SqliteDbHelper.MS_TABLE_STOREID_SEQN, value, "seqnId=?", new String[]{seqnId});
        db.close();
        return true;
    }

    public boolean UpdateSeqn(String seqnId, long seqn, int isFetched) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        ContentValues value = new ContentValues();
        value.put("seqn", seqn);
        value.put("isfetched", isFetched);
        db.update(SqliteDbHelper.MS_TABLE_STOREID_SEQN, value, "seqnId=?", new String[]{seqnId});
        db.close();
        return true;
    }

    public long SelectSeqn(String seqnId) {
        SQLiteDatabase db = mSql3Db.getReadableDatabase();
        String sqlSelect = "select seqn from " + SqliteDbHelper.MS_TABLE_STOREID_SEQN + " where seqnId='" + seqnId +"';" ;
        Cursor cursor = db.rawQuery(sqlSelect, null);
        long code = -1;
        if (cursor.moveToFirst()) {
            do {
                code = cursor.getLong(0);
            } while(cursor.moveToNext());
        }
        cursor.close();
        db.close();
        return code;
    }

    public boolean DeleteSeqn(String seqnId) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        db.delete(SqliteDbHelper.MS_TABLE_STOREID_SEQN, "seqnId=?", new String[]{seqnId});
        db.close();
        return true;
    }

    public boolean InsertGroupId(String grpId) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        ContentValues value = new ContentValues();
        value.put("seqnId", grpId);
        db.insert(SqliteDbHelper.MS_TABLE_GROUPS_ID, null, value);
        db.close();
        return true;
    }

    public ArrayList<GroupInfo> SelectGroupInfo() {
        SQLiteDatabase db = mSql3Db.getReadableDatabase();
        String sqlSelect = "select * from " + SqliteDbHelper.MS_TABLE_STOREID_SEQN +";" ;
        Cursor cursor = db.rawQuery(sqlSelect, null);
        ArrayList<GroupInfo> arrInfo = new ArrayList<GroupInfo> ();
        if (cursor.moveToFirst()) {
            do {
                HashMap<String, String> in = new HashMap<String, String>();
                GroupInfo gi = new GroupInfo();
                gi.setmSeqnId(cursor.getString(0));
                gi.setmSeqn(cursor.getLong(1));
                gi.setmIsFetched(cursor.getInt(2));
                arrInfo.add(gi);
            } while(cursor.moveToNext());
        }
        cursor.close();
        db.close();
        return arrInfo;
    }

    public boolean DeleteGroupId(String grpId) {
        SQLiteDatabase db = mSql3Db.getWritableDatabase();
        db.delete(SqliteDbHelper.MS_TABLE_GROUPS_ID, "seqnId=?", new String[]{grpId});
        db.close();
        return true;
    }

}
