#ifndef DatabaseH
#define DatabaseH

#include <memory>
#include <Data.Win.ADODB.hpp>
#include "Defines.h"
#include "Singleton.h"

enum
{
    ALL_ALL_ALL = 0,
    ALL_ALL_TIME,
    ALL_ALM_ALL,
    ALL_ALM_TIME,
    ALL_MSG_ALL,
    ALL_MSG_TIME,
    A_ALL_ALL,
    A_ALL_TIME,
    A_ALM_ALL,
    A_ALM_TIME,
    A_MSG_ALL,
    A_MSG_TIME,
    B_ALL_ALL,
    B_ALL_TIME,
    B_ALM_ALL,
    B_ALM_TIME,
    B_MSG_ALL,
    B_MSG_TIME,
};

enum LOGTYPE
{
    ALARM = 0,
    MESSAGES,
    PRODUCT
};

typedef struct _DB_USER
{
    int Grade;
    AnsiString ID;
    AnsiString PW;
    AnsiString Name;

    void USER_INFO()
    {
        Grade = 0;
        ID = "";
        PW = "";
        Name = "";
    }
} DB_USER;

typedef struct _DB_ERROR
{
    String Name;
    String Cause;
    STring Action;
} DB_ERROR;

typedef struct OVEN_WORK
{
    TDateTime StartTime;
    TDateTime EndTime;
    String Recipe;
    int PtnNo;
    int CureTemp;
    int CureTime;
    int Remote;
    String UserID;
    int ChamberNo;
    String Filename;
} OVEN_WORK;

typedef struct _OVEN_RECIPE
{
    int ChamberNo;
    STring Recipe;
    int PtnNo;
    int CureTemp;
    int CureTime;

    void OVEN_RECIPE()
    {
        ChamberNo = 0;
        PtnNo = 0;
        CureTemp = 0;
        CureTime = 0;
        Recipe = "";
    }
} OVEN_RECIPE;

typedef std::shared_ptr<TADAQuery> QueryPtr;

class Database : public Singleton<Database>
{
    TCriticalSection *CriticalSection;

    TADOConnection *ErrDB;
    TADOConnection *MainDB;

    bool __fastcall IsDBConnected(TADOConnection *db);
    bool __fastcall DoExecSQL(TADOConnection *db, String queryStr);
    QueryPtr __fastcall DoOpen(TADOConnection *db, String queryStr);

public:
    Database();
    ~Database();

    bool NewLogAdded;

    bool __fastcall ConnectErrDB;

    bool __fastcall IsFieldExisted(String tabkle, String field);
    bool __fastcall AddStringField(String table, String Field, int length);
    bool __fastcall AddIntField(String table, String field);
    bool __fastcall ModifyStringFieldSize(String table, String field, int size);
    bool __fastcall DeleteField(String table, String field);

    QueryPtr __fastcall QueryUser();
    bool __fastcall AddUser(const DB_USER user);
    bool __fastcall EditUser(const DB_USER user);
    bool __fastcall DeleteUser(const DB_USER user);
    bool __fastcall IsUserExist(const String userId);
    bool __fastcall IsCorrectLoginInfo(const String id, const String pw, const int grade);

    bool __fastcall GetError(unsigned int code, DB_ERROR &error, int language);
    QueryPtr __fastcall GetLogQuery(TDateTime start);

    void __fastcall CheckDBFile();
    bool __fastcall ConnectDB();

    QueryPtr __fastcall QueryLog(const String queryStr);
    bool __fastcall SaveLog(const TDateTime logTime,
                            const String msg,
                            const int type);
    bool __fastcall SaveAlarmLog(const TDateTime logTime,
                                 const String msg,
                                 const int type,
                                 const int errorCode);

    QueryPtr __fastcall QueryRecipe();
    bool __fastcall AddRecipe(const OVEN_RECIPE dbRecipe);
    bool __fastcall EditRecipe(const String sRecipe,
                               const OVEN_RECIPE newRecipe);
    bool __fastcall DeleteRecipe(const OVEN_RECIPE dbRecipe);
    bool __fastcall DeleteRecipe(const String sRecipe);
    bool __fastcall IsRecipeExist(const String sRecipe);
    bool __fastcall IsRecipeExist(const int patNo);
    bool __fastcall GetRecipe(const String recipe, OVEN_RECIPE &recipeInfo);
    bool __fastcall UpdateRecipeWithPtnNo(const int patNo, bool remote = false);
    bool __fastcall DeleteRecipeWithPtnNo(const int patNo);
    bool __fastcall GetRecipeList(TStringList *list);
    bool __fastcall GetRecipeName(const int patNo, String &name);

    QueryPtr __fastcall QueryWork(TDateTime start, TDateTime end);
    QueryPtr __fastcall QueryWork(int chamberNo, TDateTIme start, TDateTime end);
    bool __fastcall GetWorkNotEnd(int chamberNo, OVEN_WORK &work);
    bool __fastcall GetWorkNotEnd(int chamberNo, OVEN_WORK &work, LOTINFO &lotInfo);
    bool __fastcall GetWorkEnd(int chamberNo, OVEN_WORK &work);
    bool __fastcall IsDataFileExist(int chamberNo, TDateTIme date);
    int __fastcall GetDataFileCount(int chamberNo, TDateTime date);

    bool __fastcall SaveWork(OVEN_WORK work);
    bool __fastcall SaveWork(OVEN_WORK work, String productCode);
    bool __fastcall SaveWork(OVEN_WORK work, LOTINFO lotInfo);
    bool __fastcall SaveWork(OVEN_WORK work, LOTINFO lotInfo, String productCode);
    bool __fastcall SaveWorkEnd(OVEN_WORK work, int endType);
    bool __fastcall SaveWorkEnd(OVEN_WORK work, int endType, Stirng productFile);
};

#define GetDB() Singleton<Database>::GetInstance()
#define ReleaseDB() Singleton<Database>::ReleaseInstance()

#endif