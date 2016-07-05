//
//  MSSqlite3.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/28/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSSqlite3.h"

#define MC_MSG_SQL_DB_NAME "msgsequence.sqlite"
#define MC_MSG_SQL_TABLE_STOREID_SEQN "mc_storeid_seqn"
#define MC_MSG_SQL_TABLE_GROUPS_ID "mc_groups_id"

@implementation MSSqlite3DB

@synthesize sql3Db = _sql3Db;

- (BOOL)openDb
{
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)lastObject];
    NSString *sqlFile = [path stringByAppendingPathComponent:@MC_MSG_SQL_DB_NAME];
    int result = sqlite3_open(sqlFile.UTF8String, &_sql3Db);
    
    if (result == SQLITE_OK) {
        NSLog(@"open sqlite3 ok");
    } else {
        NSLog(@"open sqlite3 fail");
        return false;
    }
    
    // for test
    //[self dropTableName:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    //[self dropTableName:@MC_MSG_SQL_TABLE_GROUPS_ID];
    
    
    NSMutableArray *tableFields1 = [[NSMutableArray alloc] init];
    NSDictionary *item1 = [NSDictionary dictionaryWithObject:@"text unique primary key" forKey:@"seqnId"];
    NSDictionary *item2 = [NSDictionary dictionaryWithObject:@"int64" forKey:@"seqn"];
    [tableFields1 addObject:item1];
    [tableFields1 addObject:item2];
    
    NSMutableArray *tableFields2 = [[NSMutableArray alloc] init];
    NSDictionary *item3 = [NSDictionary dictionaryWithObject:@"text unique primary key" forKey:@"seqnId"];
    [tableFields2 addObject:item3];
    
    NSLog(@"------------------------------------------");
    [self createTableName:@MC_MSG_SQL_TABLE_STOREID_SEQN keyValues:tableFields1];
    NSLog(@"==========================================");
    [self createTableName:@MC_MSG_SQL_TABLE_GROUPS_ID keyValues:tableFields2];
    return true;
}

- (BOOL)closeDb
{
    if (_sql3Db)
    {
        int result = sqlite3_close(_sql3Db);
        NSLog(@"closeDb result:%d", result);
    }
    return true;
}

- (BOOL)isUserExistsInDb:(NSString*)userId
{
    NSLog(@"isUserExistsInDb was called, userId:%@", userId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    NSString *sqlSelectCount = [NSString stringWithFormat:@"select count(*) from %@ where seqnId='%@';", name, userId];
    NSLog(@"isUserExistsInDb was called, sqlSelectCount:%@", sqlSelectCount);
    int code = 0;
    int res = [self runSqlSelectCount:sqlSelectCount dbName:@MC_MSG_SQL_TABLE_STOREID_SEQN code:&code];
    NSLog(@"run sqlSqlSelectCount code:%d, res:%d", code, res);
    return ((res==0)?false:true);
}

- (BOOL)insertSeqnSeqnId:(NSString*)seqnId
                    seqn:(NSNumber*)seqn
{
    NSLog(@"insertSeqnSeqnId was called, seqnId:%@", seqnId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    NSString *key = [NSString stringWithFormat:@"'seqnId', 'seqn'"];
    NSString *value = [NSString stringWithFormat:@"\'%@\', %lld", seqnId, [seqn longLongValue]];
    NSString *sqlInsert = [NSString stringWithFormat:@"insert into %@(%@) values(%@);", name, key, value];
    NSLog(@"InsertSeqnSeqnId was called, sqlInsert:%@", sqlInsert);
    
    int code = [self runSql:sqlInsert];
    NSLog(@"run sqlInsertSeqn code:%d", code);
    return true;
}

- (BOOL)updateSeqnSeqnId:(NSString*)seqnId
                    seqn:(NSNumber*)seqn
{
    NSLog(@"updateSeqnSeqnId was called, seqnId:%@", seqnId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    NSString *key = [NSString stringWithFormat:@"'%@'", seqnId];
    NSString *value = [NSString stringWithFormat:@"%lld", [seqn longLongValue]];
    NSString *sqlUpdate = [NSString stringWithFormat:@"update %@ set seqn=%@ where seqnId=%@;", name, value, key];
    NSLog(@"UpdateSeqnSeqnId was called, sqlUpdate:%@", sqlUpdate);
    
    int code = [self runSql:sqlUpdate];
    NSLog(@"run sqlUpdateSeqn code:%d", code);
    return true;
}

- (BOOL)selectSeqnSeqnId:(NSString*)seqnId
                    seqn:(NSNumber**)seqn
{
    if (!seqn) return false;
    NSLog(@"selectSeqnSeqnId was called, seqnId:%@", seqnId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    NSString *key = [NSString stringWithFormat:@"seqnId='%@'", seqnId];
    NSString *sqlSelect = [NSString stringWithFormat:@"select seqn from %@ where %@;", name, key];
    NSLog(@"SelectSeqnSeqnId was called, sqlSelect:%@", sqlSelect);
    int code = 0;
    *seqn = [self runSqlSelect:sqlSelect dbName:name code:&code];
    return true;
}

- (BOOL) deleteSeqnSeqnId:(NSString*)seqnId
{
    NSLog(@"deleteSeqnSeqnId was called, seqnId:%@", seqnId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_STOREID_SEQN];
    NSString *key = [NSString stringWithFormat:@"seqnId='%@'", seqnId];
    NSString *sqlDelete = [NSString stringWithFormat:@"delete from %@ where %@;", name, key];
    NSLog(@"DeleteSeqnSeqnId was called, sqlInsert:%@", sqlDelete);
    
    int code = [self runSql:sqlDelete];
    NSLog(@"run sqlDeleteSeqn code:%d", code);
    return true;
}


- (BOOL)insertGroupIdGrpId:(NSString*)grpId
{
    NSLog(@"insertGroupIdGrpId was called, groupId:%@", grpId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_GROUPS_ID];
    NSString *key = [NSString stringWithFormat:@"'seqnId'"];
    NSString *value = [NSString stringWithFormat:@"'%@'", grpId];
    NSString *sqlInsert = [NSString stringWithFormat:@"insert into %@(%@) values(%@);", name, key, value];
    NSLog(@"InsertGroupIdGrpId was called, sqlInsert:%@", sqlInsert);
    
    int code = [self runSql:sqlInsert];
    NSLog(@"run sqlInsertGroupId code:%d", code);
    return true;
}

- (BOOL)selectGroupIds:(NSMutableArray**)Ids
{
    if (!Ids) return false;
    NSLog(@"selectGroupIds was called");
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_GROUPS_ID];
    NSString *sqlSelect = [NSString stringWithFormat:@"select seqnId from %@;", name];
    NSLog(@"SelectGroupIds was called, sqlSelect:%@", sqlSelect);
    int code = 0;
    *Ids = [self runSqlSelect:sqlSelect dbName:name code:&code];
    return true;
}

- (BOOL)deleteGroupIdGrpId:(NSString*)grpId
{
    NSLog(@"deleteGroupIdGrpId was called, groupId:%@", grpId);
    NSString *name = [NSString stringWithFormat:@MC_MSG_SQL_TABLE_GROUPS_ID];
    NSString *key = [NSString stringWithFormat:@"seqnId='%@'", grpId];
    NSString *sqlDelete = [NSString stringWithFormat:@"delete from %@ where %@;", name, key];
    NSLog(@"DeleteGroupIdGrpId was called, sqlDelete:%@", sqlDelete);
    
    int code = [self runSql:sqlDelete];
    NSLog(@"run sqlDeleteGroupId code:%d", code);
    return true;
}


/////////////////////////////////////////
////////////////private//////////////////
/////////////////////////////////////////

- (void)createTableName:(NSString*)name keyValues:(NSMutableArray*)keyValues
{
    NSMutableString *keys = [[NSMutableString alloc] init];
    int index = 1;
    for (NSDictionary *item in keyValues) {
        NSString *key = [[item allKeys] firstObject];
        NSString *value = [item objectForKey:key];
        if (index < keyValues.count)
        {
            [keys appendFormat:@"%@ %@,", key, value];
        } else if (index == keyValues.count) {
            [keys appendFormat:@"%@ %@", key, value];
        }
        index++;
    }
    NSLog(@"createTable was called, name:%@", name);
    NSString *sqlCreateTable = [NSString stringWithFormat:@"create table if not exists %@(%@);", name, keys];
    NSLog(@"createTable was called, sqlCreateTable:%@", sqlCreateTable);
    int code = [self runSql:sqlCreateTable];
    NSLog(@"run sqlCreateTable code:%d", code);
}

- (void)dropTableName:(NSString*)name
{
    NSLog(@"dropTable was called, name:%@", name);
    NSString *sqlDropTable = [NSString stringWithFormat:@"drop table %@;", name];
    NSLog(@"dropTable was called, sqlDropTable:%@", sqlDropTable);
    int code = [self runSql:sqlDropTable];
    NSLog(@"run sqldropTable code:%d", code);
}

- (int)runSql:(NSString*)sql
{
    if (!_sql3Db)
    {
        NSLog(@"runSql _sql3Db is null");
        return -1;
    }
    NSLog(@"runSql was called");
    char *errmsg = nil;
    int code = sqlite3_exec(_sql3Db, [sql UTF8String], NULL, NULL, &errmsg);
    if (code != SQLITE_OK)
    {
        if (errmsg)
        NSLog(@"run Sql code :%d, errmsg %@", code, [NSString stringWithCString:errmsg encoding:NSUTF8StringEncoding]);
        else
        NSLog(@"run Sql code :%d, error", code);
    }
    return code;
}

- (void)runSql:(NSString*)sql callback:(void(^)(int error))complete
{
    if (!_sql3Db)
    {
        NSLog(@"runSql _sql3Db is null");
        return;
    }
    NSLog(@"runSql was called");
    char *errmsg = nil;
    int code = sqlite3_exec(_sql3Db, [sql UTF8String], NULL, NULL, &errmsg);
    if (code != SQLITE_OK)
    {
        if (errmsg)
            NSLog(@"run Sql with cb code :%d, errmsg %@", code, [NSString stringWithCString:errmsg encoding:NSUTF8StringEncoding]);
        else
            NSLog(@"run Sql with cb code :%d, error", code);
        complete(code);
    } else {
        NSLog(@"run Sql SQLITE_OK");
        complete(SQLITE_OK);
    }
}

- (id<NSObject>)runSqlSelect:(NSString*)sql
                      dbName:(NSString*)dbName
                        code:(int*)code
{
    if (!_sql3Db || !code)
    {
        NSLog(@"runSqlSelect _sql3Db is null");
        return nil;
    }
    NSLog(@"runSqlSelect was called");
    sqlite3_stmt *stmt = nil;
    *code = sqlite3_prepare_v2(_sql3Db, [sql UTF8String], -1, &stmt, NULL);
    if (*code != SQLITE_OK)
    {
        NSLog(@"run Sql Select err code %d", *code);
    } else {
        NSLog(@"run Sql Select SQLITE_OK");
        if ([dbName isEqualToString:@MC_MSG_SQL_TABLE_STOREID_SEQN])
        {
            NSNumber *num = nil;
            while(sqlite3_step(stmt) == SQLITE_ROW)
            {
                sqlite3_int64 seqn = sqlite3_column_int64(stmt, 0);
                NSLog(@"select seqn :%lld", seqn);
                num = [NSNumber numberWithLongLong:seqn];
                break;
            }
            sqlite3_finalize(stmt);
            stmt = nil;
            return num;
        } else if ([dbName isEqualToString:@MC_MSG_SQL_TABLE_GROUPS_ID])
        {
            NSMutableArray  *arr = [[NSMutableArray alloc] init];
            while(sqlite3_step(stmt) == SQLITE_ROW)
            {
                char *grpid = (char*)sqlite3_column_text(stmt, 0);
                NSString *nsGrpid = [NSString stringWithCString:grpid encoding:NSUTF8StringEncoding];
                sqlite3_int64 seqn = sqlite3_column_int64(stmt, 1);
                NSNumber * nsSeqn = [NSNumber numberWithLongLong:seqn];
                NSDictionary *item = [NSDictionary dictionaryWithObject:nsSeqn forKey:nsGrpid];
                [arr addObject:item];
                NSLog(@"select grpid:%@, seqn:%lld", nsGrpid, [nsSeqn longLongValue]);
            }
            sqlite3_finalize(stmt);
            stmt = nil;
            return arr;
        }
    }
    return nil;
}

- (int)runSqlSelectCount:(NSString*)sql
                      dbName:(NSString*)dbName
                        code:(int*)code
{
    int count = 0;
    if (!_sql3Db || !code)
    {
        NSLog(@"runSqlSelectCount _sql3Db is null");
        return count;
    }
    NSLog(@"runSqlSelectCount was called");
    sqlite3_stmt *stmt = nil;
    *code = sqlite3_prepare_v2(_sql3Db, [sql UTF8String], -1, &stmt, NULL);
    if (*code != SQLITE_OK)
    {
        NSLog(@"run Sql SelectCount err code %d", *code);
        return count;
    } else {
        NSLog(@"run Sql SelectCount SQLITE_OK");
        while(sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
            NSLog(@"runSqlSelectCount count:%d", count);
        }
        sqlite3_finalize(stmt);
        stmt = nil;
        return count;
    }
}

@end
