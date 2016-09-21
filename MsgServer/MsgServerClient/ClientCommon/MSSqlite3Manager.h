//
//  MSSqlite3Manager.h
//  SdkMsgClientIos
//
//  Created by hp on 7/1/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSqlite3Manager_h
#define MSSqlite3Manager_h

#include <string>
#include <map>
#include "MSSqlite3.h"

class MSSqlite3Manager {
public:
    MSSqlite3Manager():_sqlite3Db(nullptr){}
    ~MSSqlite3Manager(){}
    void InitManager(const std::string& userId);
    void UninManager();
    bool IsUserExists(const std::string& userId, const std::string& seqnId);
    bool IsGroupExists(const std::string& userId, const std::string& grpId);
    void AddUser(const std::string& userId);
    void UpdateUserSeqn(const std::string& userId, ::google::protobuf::int64 seqn);
    void GetUserSeqn(const std::string& userId, ::google::protobuf::int64* seqn);
    void AddGroupId(const std::string& userId, const std::string& groupId);
    void AddGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn);
    void UpdateGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn);
    void UpdateGroupInfo(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn, int isfetched);
    void GetGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64* seqn);
    void GetGroupInfo(const std::string& userId, DbResultVector& info);
    void DelGroupId(const std::string& userId, const std::string& groupId);

private:
    MSSqlite3DB *_sqlite3Db;
};

#endif /* MSSqlite3Manager_h */
