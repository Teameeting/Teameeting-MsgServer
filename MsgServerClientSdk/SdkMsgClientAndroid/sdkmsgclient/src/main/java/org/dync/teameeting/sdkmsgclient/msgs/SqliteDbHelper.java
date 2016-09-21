package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * Created by hp on 7/11/16.
 */
public class SqliteDbHelper extends SQLiteOpenHelper{

    // 数据库版本号
    private static final int MS_DATABASE_VERSION = 1;
    // 数据库名
    private static final String MS_DATABASE_NAME = "msgsequence.db";

    public static final String MS_TABLE_STOREID_SEQN = "mc_storeid_seqn";

    public static final String MS_TABLE_GROUPS_ID = "mc_groups_id";

    public SqliteDbHelper(Context context) {
        super(context, MS_DATABASE_NAME, null, MS_DATABASE_VERSION);
    }

    @Override
    public void onOpen(SQLiteDatabase db) {
        super.onOpen(db);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("create table if not exists " + MS_TABLE_STOREID_SEQN + "(userId varchar(512), seqnId varchar(512), seqn bigint default 0, isfetched int default 0, primary key(userId, seqnId))");
        db.execSQL("create table if not exists " + MS_TABLE_GROUPS_ID + "(userId varchar(512), seqnId varchar(512), primary key(userId, seqnId))");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + MS_TABLE_GROUPS_ID);
        db.execSQL("DROP TABLE IF EXISTS " + MS_TABLE_STOREID_SEQN);
        onCreate(db);
    }
}
