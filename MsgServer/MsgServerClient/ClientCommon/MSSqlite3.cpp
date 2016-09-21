//
//  MSSqlite3.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/28/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include <string>
#include <sstream>
#include <utility>
#include "MSSqlite3.h"
#include "rtklog.h"

#define MC_MSG_SQL_DB_NAME "msgsequence.sqlite"
#define MC_MSG_SQL_TABLE_STOREID_SEQN "mc_storeid_seqn"
#define MC_MSG_SQL_TABLE_GROUPS_ID "mc_groups_id"

bool MSSqlite3DB::OpenDb()
{
    std::string path("./");
    std::string sqlFile = path.append(MC_MSG_SQL_DB_NAME);
    printf("OpenDb sqlFile path:%s\n", sqlFile.c_str());
    int result = sqlite3_open(sqlFile.c_str(), &_sql3Db);

    if (result == SQLITE_OK) {
        LI("open sqlite3 ok\n");
    } else {
        LI("open sqlite3 fail\n");
        return false;
    }

    // for test
    //DropTable(MC_MSG_SQL_TABLE_STOREID_SEQN);
    //DropTable(MC_MSG_SQL_TABLE_GROUPS_ID);


    DbResultVector tableFields1;
    DbFieldMap item11, item12, item13, item14, item15;

    item11.insert(std::make_pair("userId"     , "text default ''"));
    item12.insert(std::make_pair("seqnId"     , "text default ''"));
    item13.insert(std::make_pair("seqn"       , "int64 default 0"));
    item14.insert(std::make_pair("isfetched"  , "int default 0"));
    item15.insert(std::make_pair("composeKey" , "primary key(userId,seqnId)"));

    tableFields1.push_back(item11);
    tableFields1.push_back(item12);
    tableFields1.push_back(item13);
    tableFields1.push_back(item14);
    tableFields1.push_back(item15);

    DbResultVector tableFields2;
    DbFieldMap item21, item22, item23;

    item21.insert(std::make_pair("userId"     , "text default ''"));
    item22.insert(std::make_pair("seqnId"     , "text default ''"));
    item23.insert(std::make_pair("composeKey" , "primary key(userId, seqnId)"));

    tableFields2.push_back(item21);
    tableFields2.push_back(item22);
    tableFields2.push_back(item23);

    LI("------------------------------------------");
    CreateTable(MC_MSG_SQL_TABLE_STOREID_SEQN, tableFields1);
    LI("==========================================");
    CreateTable(MC_MSG_SQL_TABLE_GROUPS_ID, tableFields2);
    return true;
}

bool MSSqlite3DB::CloseDb()
{
    if (_sql3Db)
    {
        int result = sqlite3_close(_sql3Db);
        LI("closeDb result:%d\n", result);
    }
    return true;
}

bool MSSqlite3DB::IsUserExistsInDb(const std::string& userId, const std::string& seqnId)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "select count(*) from " << name << " where userId='" << userId << "' and seqnId='" << seqnId << "';";
    std::string sqlSelectCount = oss.str();
    LI("isUserExistsInDb was called, sqlSelectCount:%s\n", sqlSelectCount.c_str());

    int code = 0;
    int res = RunSqlSelectCount(sqlSelectCount, MC_MSG_SQL_TABLE_STOREID_SEQN, &code);
    return ((res==0)?false:true);
}

// isfetched default is 0
bool MSSqlite3DB::InsertSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::string key = "'userId','seqnId','seqn','isfetched'";
    std::ostringstream oss;
    oss << "insert into " << name << "(" << key << ") values('" << userId << "','" << seqnId << "'," << seqn << "," << 0 << ");";
    std::string sqlInsert = oss.str();
    LI("insertSeqnSeqnId was called, sqlInsert:%s\n", sqlInsert.c_str());

    int code = RunSql(sqlInsert);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::UpdateSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "update " << name << " set seqn=" << seqn << " where userId='" << userId << "' and seqnId='" << seqnId << "';";
    std::string sqlUpdate = oss.str();
    LI("updateSeqnSeqnId was called, sqlUpdate:%s\n", sqlUpdate.c_str());

    int code = RunSql(sqlUpdate);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::UpdateSeqnWithFetched(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn, int isfetched)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "update " << name << " set seqn=" << seqn << ", isfetched=" << isfetched << " where userId='" << userId << "' and seqnId='" << seqnId << "';";
    std::string sqlUpdate = oss.str();
    LI("updateSeqnSeqnId with fetched was called, sqlUpdate:%s\n", sqlUpdate.c_str());

    int code = RunSql(sqlUpdate);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::SelectSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64* seqn)
{
    if (!seqn) return false;
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "select seqn from " << name << " where userId='" << userId << "' and seqnId='" << seqnId << "';";
    std::string sqlSelect = oss.str();
    LI("selectSeqnSeqnId was called, sqlSelect:%s\n", sqlSelect.c_str());
    int code = 0;
    DbResultVector vec;
    code = RunSqlSelect(sqlSelect, name, 1, vec);
    DbFieldMap map = vec[0];
    ::google::protobuf::int64 st = std::stoll(map.begin()->second);
    *seqn = st;
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::DeleteSeqn(const std::string& userId, const std::string& seqnId)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "delete from " << name << " where userId='" << userId << "' and seqnId='" << seqnId << "';";
    std::string sqlDelete = oss.str();
    LI("deleteSeqnSeqnId was called, sqlInsert:%s\n", sqlDelete.c_str());

    int code = RunSql(sqlDelete);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::InsertGroupId(const std::string& userId, const std::string& grpId)
{
    std::string name = MC_MSG_SQL_TABLE_GROUPS_ID;
    std::string key = "'userId', 'seqnId'";
    std::ostringstream oss;
    oss << "insert into " << name << "(" << key << ") values('" << userId << "','" << grpId << "');";
    std::string sqlInsert = oss.str();
    LI("insertGroupIdGrpId was called, sqlInsert:%s\n", sqlInsert.c_str());

    int code = RunSql(sqlInsert);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::IsGroupExists(const std::string& userId, const std::string& grpId)
{
    std::string name = MC_MSG_SQL_TABLE_GROUPS_ID;
    std::ostringstream oss;
    oss << "select count(*) from " << name << " where userId='" << userId << "';";
    std::string sqlSelectCount = oss.str();
    LI("isGroupExistsInDb was called, sqlSelectCount:%s\n", sqlSelectCount.c_str());
    int code = 0;
    int res = RunSqlSelectCount(sqlSelectCount, MC_MSG_SQL_TABLE_GROUPS_ID, &code);
    return ((res==0)?false:true);
}

bool MSSqlite3DB::SelectGroupInfo(const std::string& userId, DbResultVector& info)
{
    std::string name = MC_MSG_SQL_TABLE_STOREID_SEQN;
    std::ostringstream oss;
    oss << "select * from " << name << " where userId='" << userId << "';";
    std::string sqlSelect = oss.str();
    LI("selectGroupIds was called, sqlSelect:%s\n", sqlSelect.c_str());
    int code = 0;
    code = RunSqlSelect(sqlSelect, name, 2, info);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}

bool MSSqlite3DB::DeleteGroupId(const std::string& userId, const std::string& grpId)
{
    std::string name = MC_MSG_SQL_TABLE_GROUPS_ID;
    std::ostringstream oss;
    oss << "delete from " << name << " where userId='" << userId << "' and seqnId='" << grpId << "';";
    std::string sqlDelete = oss.str();
    LI("deleteGroupIdGrpId was called, sqlDelete:%s\n", sqlDelete.c_str());

    int code = RunSql(sqlDelete);
    LI("func:%s code:%d\n", __FUNCTION__, code);
    return true;
}


/////////////////////////////////////////
////////////////private//////////////////
/////////////////////////////////////////

void MSSqlite3DB::CreateTable(const std::string& name, DbResultVector& keyValues)
{
    std::string keys("");
    int index = 1;
    for (DbFieldMap item : keyValues) {
        DbFieldMapIt it = item.begin();
        std::string key = it->first;
        std::string value = it->second;
        if (index < (int)keyValues.size())
        {
            keys.append(key);
            keys.append(" ");
            keys.append(value);
            keys.append(",");
        } else if (index == (int)keyValues.size()) {
            keys.append(" ");
            keys.append(value);
        }
        index++;
    }
    std::ostringstream oss;
    oss << "create table if not exists " << name << "(" << keys << ");";
    std::string sqlCreateTable = oss.str();
    LI("createTable was called, sqlCreateTable:%s\n", sqlCreateTable.c_str());
    int code = RunSql(sqlCreateTable);
    LI("func:%s code:%d\n", __FUNCTION__, code);
}

void MSSqlite3DB::DropTable(const std::string& name)
{
    std::ostringstream oss;
    oss << "drop table " << name << ";";
    std::string sqlDropTable = oss.str();
    LI("dropTable was called, sqlDropTable:%s\n", sqlDropTable.c_str());
    int code = RunSql(sqlDropTable);
    LI("func:%s code:%d\n", __FUNCTION__, code);
}

int MSSqlite3DB::RunSql(const std::string& sql)
{
    if (!_sql3Db)
    {
        LI("runSql _sql3Db is null\n");
        return -1;
    }
    char *errmsg = nullptr;
    int code = sqlite3_exec(_sql3Db, sql.c_str(), NULL, NULL, &errmsg);
    if (code != SQLITE_OK)
    {
        if (errmsg)
        LI("run Sql code :%d, errmsg: %s\n", code, errmsg);
        else
        LI("run Sql code :%d, ok\n", code);
    }
    return code;
}

int MSSqlite3DB::RunSqlSelect(const std::string& sql, const std::string& dbName, int type, DbResultVector& result)
{
    if (!_sql3Db)
    {
        LI("runSqlSelect _sql3Db is null\n");
        return -1;
    }
    int code = 0;
    sqlite3_stmt *stmt = nullptr;
    code = sqlite3_prepare_v2(_sql3Db, sql.c_str(), -1, &stmt, NULL);
    if (code != SQLITE_OK)
    {
        LI("run Sql Select err code %d\n", code);
        return -1;
    } else {
        LI("run Sql Select SQLITE_OK\n");
        if (dbName.compare(MC_MSG_SQL_TABLE_STOREID_SEQN)==0)
        {
            if (type == 1)// seqn
            {
                ::google::protobuf::int64 num = 0;
                while(sqlite3_step(stmt) == SQLITE_ROW)
                {
                    sqlite3_int64 seqn = sqlite3_column_int64(stmt, 0);
                    LI("select seqn :%lld\n", seqn);
                    num = seqn;
                    break;
                }
                sqlite3_finalize(stmt);
                stmt = nullptr;
                DbFieldMap map;
                std::string strNum = std::to_string(num);
                map.insert(std::make_pair("seqn", strNum));
                result.push_back(map);
                return code;
            } else if (type ==2)// group info
            {
                while(sqlite3_step(stmt) == SQLITE_ROW)
                {
                    char *userid = (char*)sqlite3_column_text(stmt, 0);
                    char *grpid = (char*)sqlite3_column_text(stmt, 1);
                    sqlite3_int64 seqn = sqlite3_column_int64(stmt, 2);
                    int fetched = sqlite3_column_int(stmt, 3);
                    std::string nsUserId = userid;
                    std::string nsGrpId = grpid;
                    std::string nsSeqn = std::to_string(seqn);
                    std::string nsFetched = std::to_string(fetched);

                    DbFieldMap map;

                    map.insert(std::make_pair("userId" , nsUserId));
                    map.insert(std::make_pair("seqnId" , nsGrpId));
                    map.insert(std::make_pair("seqn"   , nsSeqn));
                    map.insert(std::make_pair("isfetched" , nsFetched));

                    result.push_back(map);

                    LI("select userid:%s, grpid:%s, seqn:%s, isfetched:%s\n", nsUserId.c_str(), nsGrpId.c_str(), nsSeqn.c_str(), nsFetched.c_str());
                }
                sqlite3_finalize(stmt);
                stmt = nullptr;
                return code;
            }

        } else if (dbName.compare(MC_MSG_SQL_TABLE_GROUPS_ID)==0)
        {
            while(sqlite3_step(stmt) == SQLITE_ROW)
            {
                char *grpid = (char*)sqlite3_column_text(stmt, 0);
                std::string nsGrpId = grpid;
                DbFieldMap map;
                map.insert(std::make_pair("grpid", nsGrpId));
                result.push_back(map);
                LI("select get grpid:%s\n", nsGrpId.c_str());
            }
            sqlite3_finalize(stmt);
            stmt = nullptr;
            return code;
        }
    }
    return -1;
}

int MSSqlite3DB::RunSqlSelectCount(const std::string& sql, const std::string& dbName, int* code)
{
    int count = 0;
    if (!_sql3Db || !code)
    {
        LI("runSqlSelectCount _sql3Db is null\n");
        return count;
    }
    sqlite3_stmt *stmt = nullptr;
    *code = sqlite3_prepare_v2(_sql3Db, sql.c_str(), -1, &stmt, NULL);
    if (*code != SQLITE_OK)
    {
        LI("run Sql SelectCount err code %d\n", *code);
        return count;
    } else {
        while(sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
            LI("runSqlSelectCount count:%d\n", count);
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
        return count;
    }
}

