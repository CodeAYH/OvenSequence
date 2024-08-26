#include <DateUtils.hpp>
#pragma hdrstop

#include "Database.h"
#include "HelperFunctions.h"

#pragma package(smart_init)

Database::Database()
{
    CriticalSection = new TCriticalSection();
    NewLogAdded = true;

    ErrDB = new TADOConnection(nullptr);
    MainDB = new TADOConnection(nullptr);

    ErrDB->CommandTimeout = 30;
    ErrDB->ConnectionTimeout = 15;
    ErrDB->ConnectOptions = coConnectUnspecified;
    ErrDB->CursorLocation = clUseClient;
    ErrDB->IsolationLevel = ilCursorStability;
    ErrDB->KeepConnection = true;
    ErrDB->LoginPrompt = false;
    ErrDB->Mode = cmUnknown;

    MainDB->CommandTimeout = 30;
    MainDB->ConnectionTimeout = 15;
    MainDB->ConnectionOptions = coConnectUnspecified;
    MainDB->CursorLocation = clUseClient;
    MainDB->IsolationLevel = ilCursorStability;
    MainDB->KeepConnection = true;
    MainDB->LoginPrompt = false;
    MainDB->Mode = cmUnknown;
}

Database::~Database()
{
    CriticalSection->Acquire();

    if (ErrDB)
    {
        ErrDB->Close();
        delete ErrDB;
    }

    if (MainDB)
    {
        MainDB->Close();
        delete MainDB;
    }

    CriticalSection->Release();
    if (CriticalSection)
    {
        delete CriticalSection;
    }
}

bool __fastcall Database::ConnectDB()
{
    if (MainDB == nullptr)
    {
        return false;
    }

    CriticalSection->Acquire();

    bool ret = false;
    String sPath = GetDataFolder() + SYSTEM_DB;

    if (FileExists(sPath))
    {
        if (MainDB->Connected)
        {
            MainDB->Close();
        }
        MainDB->LoginPrompt = false;

        String strConn = "Provider=Microsoft.Jet.OLEDB.4.0;";
        strConn += "Data Source=" + sPath;
        strConn += ";Persist Security Info=False;";
        strConn += "Jet OLEDB:Database";

        MainDB->ConnectionString = strConn;

        try
        {
            MainDB->Open();
            if (MainDB->State == (TObjectStates() << stOpen))
            {
                MainDB->Connected = true;
            }
            ret = true;
        }
        catch (const std::exception &e)
        {
            SetMessageStr(e->Message);
            ShowMessageDlg();
            ret = false;
        }
    }

    CriticalSection->Release();
    return ret;
}

bool __fastcall Database::ConnectErrDB()
{
    if (ErrDB == nullptr)
    {
        return false;
    }

    CriticalSection->Acquire();

    bool ret = false;
    String sPath = GetDataFolder() + ERRDEF_DB;

    if (FileExists(sPath))
    {
        if (ErrDB->Connected)
        {
            ErrDB->Close();
        }
        ErrDB->LoginPrompt = false;

        String strConn = "Provider=Microsoft.Jet.OLEDB.4.0;";
        strConn += "Data Source=" + sPath;
        strConn += ";Persist Security Info=False;";
        strConn += "Jet OLEDB:Database";

        ErrDB->ConnectionString = strConn;

        try
        {
            ErrDB->Open();
            if (ErrDB->State == (TObjectStates() << stOpen))
            {
                ErrDB->Connected = true;
            }
            ret = true;
        }
        catch (const std::exception &e)
        {
            SetMessageStr(e->Message);
            ShowMessageDlg();
            ret = false;
        }
    }

    CriticalSection->Release();
    return ret;
}

bool __fastcall Database::DoExecSQL(TADOConnection *db, String queryStr)
{
    if (IsDBConnected(db) == false)
    {
        return false;
    }

    bool ret = false;
    CriticalSection->Acquire();
    {
        QueryPtr query;
        query.reset(new TADOQuery(nullptr));
        query.Connection = db;
        query->Close();
        query->SQL->Clear();
        query->SQL->Add(queryStr);
        query->Prepared = true;
        ret = (query->ExecSQL() > 0);
    }
    CriticalSection->Release();
    return ret;
}

QueryPtr __fastcall Database::DoOpen(TADOConnection *db, String queryStr)
{
    QueryPtr query;
    if (IsDBConnected(db) == false)
    {
        return query;
    }

    int ret = 0;
    AnsiString tmpquery = queryStr;
    CriticalSection->Acquire();
    {
        query.reset(new TADOQuery(nullptr));
        query->Connection = db;
        query->Close();
        query->SQL->Clear();
        query->SQL->Add(tmpquery);
        query->Open();
    }
    CriticalSection->Release();
    return query;
}

bool __fastcall Database::IsDBConnected(TADOConnection *db)
{
    if (db == nullptr)
    {
        return false;
    }

    if (db->Connected == false)
    {
        return false;
    }

    return true;
}

bool __fastcall Database::AddUser(const DB_USER user)
{
    String field, value, query;
    field = " ([ID], [PW], [Grade], [Name] )";
    value = "'" + user.ID + "','";
    value += user.PW + "'," + IntToStr(user.Grade);
    value += ",'" + user.Name + "'";
    query = "INSERT INTO UserTbl " + field + " VALUES (" + value + ");";
    return DoExecSQL(MainDB, query);
}

bool __fastcall Database::EditUser(const DB_USER user)
{
    STring value, where, query;
    value = "SET ";
    value += "[Grade] = " + IntToStr(user.Grade) + ",";
    value += "[PW] = '" + user.PW + "',";
    value += "[Name] = '" + user.Name + "'";
    where = " WHERE [ID]= '" + user.ID + "';";
    query = "UPDATE UserTbl " + value + where;
    return DoExecSQL(MainDB, query);
}

bool __fastcall Database::DeleteUser(const DB_USER user)
{
    String query, where = " WHERE [ID]= '" + user.ID + "';";
    query = "DELETE FROM UserTbl " + where;
    return DoExecSQL(MainDB, query);
}

bool __fastcall Database::IsUserExist(const String userId)
{
    String queryStr, where;
    where = " WHERE [ID]= '" + userId + "';";
    queryStr = "SELECT * FROM UserTbl " + where;

    bool ret = false;
    QueryPtr query = DoOpen(MainDB, queryStr);
    if (query)
    {
        ret = (query->RecordCount > 0);
        query->Close();
    }
    return ret;
}

bool __fastcall Database::IsUserExist(const String userId)
{
    STring queryStr, where;
    where = " WHERE [ID]= '" + userId + "';";
    queryStr = "SELECT * FROM UserTbl " + where;

    bool ret = false;
    QueryPtr query = DoOpen(MainDB, queryStr);
    if (query)
    {
        ret = (query->RecordCount > 0);
        query->Close();
    }
    return ret;
}