//
//  MSSqlite3.h
//  SdkMsgClientIos
//
//  Created by hp on 6/28/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSqlite3_h
#define MSSqlite3_h

#include <vector>
#include <map>
#include <string>
#include "sqlite3.h"
#include "proto/storage_msg.pb.h"
#include "proto/storage_msg_type.pb.h"
#include "proto/entity_msg_type.pb.h"
#include "proto/entity_msg.pb.h"

typedef std::map<std::string, std::string>      DbFieldMap;
typedef DbFieldMap::iterator                    DbFieldMapIt;
typedef std::vector<DbFieldMap>                 DbResultVector;
typedef DbResultVector::iterator                DbResultVectorIt;

class MSSqlite3DB {
public:

    bool OpenDb();
    bool CloseDb();
    bool IsUserExistsInDb(const std::string& userId, const std::string& seqnId);
    bool InsertSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn);
    bool UpdateSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn);
    bool UpdateSeqnWithFetched(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64 seqn, int isfetched);
    bool SelectSeqn(const std::string& userId, const std::string& seqnId, ::google::protobuf::int64* seqn);
    bool DeleteSeqn(const std::string& userId, const std::string& seqnId);
    bool InsertGroupId(const std::string& userId, const std::string& grpId);
    bool IsGroupExists(const std::string& userId, const std::string& grpId);
    bool SelectGroupInfo(const std::string& userId, DbResultVector& info);
    bool DeleteGroupId(const std::string& userId, const std::string& grpId);

private:
    void CreateTable(const std::string& name, DbResultVector& keyValues);
    void DropTable(const std::string& name);

    int RunSql(const std::string& sql);

    int RunSqlSelect(const std::string& sql, const std::string& dbName, int type, DbResultVector& result);
    int RunSqlSelectCount(const std::string& sql, const std::string& dbName, int* code);
private:
    sqlite3* _sql3Db;
};

#endif /* MSSqlite3_h */
