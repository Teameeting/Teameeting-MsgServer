//
//  MSSqlite3Manager.mm
//  SdkMsgClientIos
//
//  Created by hp on 7/1/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSSqlite3Manager.h"

void MSSqlite3Manager::InitManager(const std::string& userId)
{
    _sqlite3Db = new MSSqlite3DB();
    if (_sqlite3Db)
    {
        _sqlite3Db->OpenDb(userId);
    }
}

void MSSqlite3Manager::UninManager()
{
    if (_sqlite3Db)
    {
        _sqlite3Db->CloseDb();
        delete _sqlite3Db;
        _sqlite3Db = nullptr;
    }
}

bool MSSqlite3Manager::IsUserExists(const std::string& userId, const std::string& seqnId)
{
    return _sqlite3Db->IsUserExistsInDb(userId, seqnId);
}

bool MSSqlite3Manager::IsGroupExists(const std::string& userId, const std::string& grpId)
{
    return _sqlite3Db->IsGroupExists(userId, grpId);
}

void MSSqlite3Manager::AddUser(const std::string& userId)
{
    if(_sqlite3Db)
    {
        _sqlite3Db->InsertSeqn(userId, userId, 0);
    }
}

void MSSqlite3Manager::UpdateUserSeqn(const std::string& userId, ::google::protobuf::int64 seqn)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->UpdateSeqn(userId, userId, seqn);
    }
}

void MSSqlite3Manager::GetUserSeqn(const std::string& userId, ::google::protobuf::int64* seqn)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->SelectSeqn(userId, userId, seqn);
    }
}

void MSSqlite3Manager::AddGroupId(const std::string& userId, const std::string& groupId)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->InsertGroupId(userId, groupId);
    }
}

void MSSqlite3Manager::AddGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->InsertSeqn(userId, grpId, seqn);
    }
}

void MSSqlite3Manager::UpdateGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->UpdateSeqn(userId, grpId, seqn);
    }
}

void MSSqlite3Manager::UpdateGroupInfo(const std::string& userId, const std::string& grpId, ::google::protobuf::int64 seqn, int isfetched)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->UpdateSeqnWithFetched(userId, grpId, seqn, isfetched);
    }
}

void MSSqlite3Manager::GetGroupSeqn(const std::string& userId, const std::string& grpId, ::google::protobuf::int64* seqn)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->SelectSeqn(userId, grpId, seqn);
    }
}

void MSSqlite3Manager::GetGroupInfo(const std::string& userId, DbResultVector& info)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->SelectGroupInfo(userId, info);
    }
}

void MSSqlite3Manager::DelGroupId(const std::string& userId, const std::string& groupId)
{
    if (_sqlite3Db)
    {
        _sqlite3Db->DeleteSeqn(userId, groupId);
        _sqlite3Db->DeleteGroupId(userId, groupId);
    }
}
