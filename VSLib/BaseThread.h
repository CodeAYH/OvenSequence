#ifndef BaseThreadH
#define BaseThreadH

#include <System.Classes.hpp>
#include <map>
#include <vector>

using namespace std;

class BaseThread : public TThread
{
public:
    enum INIT_STATUS
    {
        IS_NOT_INIT = 0,
        IS_INITIALIZING,
        IS_INIT_DONE,
        IS_COUNT
    };

protected:
    bool StepChanged;
    bool FEnabled;
    int FEQType;
    int FModuleType;
    int FStep;
    int FInitStatus;
    int FRunStatus;
    int FPrevRunStatus;

    bool FInitEnd;
    bool FInitialized;

    map<int, String> FStepString;
    vector<int> AlarmList;
    String ModuleName;

protected:
    virtual bool __fastcall IsAutoRun() = 0;
    virtual bool __fastcall IsPaused() = 0;
    virtual void __fastcall DoWorkSequence() = 0;
    virtual void __fastcall Execute();
    virtual void __fastcall LogStepChanged();
    virtual void __fastcall SetStep(int step);
    virtual void __fastcall DelayTime(unsigned int milliSeconds, bool workInThread = true);
    virtual int __fastcall GetRunStatus();
    virtual void __fastcall SetRunStatus(int status);
    virtual void __fastcall Initialize();

public:
    __property bool Enabled = {read = FEnabled, write = FEnabled};
    __property int EQType = {read = FEQType, write = FEQType};
    __property int ModuleType = {read = FModuleType, write = FModuleType};
    __property int InitStatus = {read = FInitStatus, write = FInitStatus};
    __property int RunStatus = {read = GetRunStatus, write = SetRunStatus};
    __property int PrevRunStatus = {read = FPrevRunStatus, write = FPrevRunStatus};
    __property int Step = {read = FStep, write = FStep};
    __property int StepString = {read = GetStepString};

public:
    __fastcall BaseThread(int moduleType);
    __fastcall ~BaseThread();

    virtual String __fastcall GetModuleName() { return ModuleName; }
    virtual String __fastcall GetStepString();
    virtual void __fastcall RaiseAlarm(int errorCode, bool setPaused = false) = 0;

    virtual void __fastcall ResumeRunStatus();

    virtual void __fastcall AddAlarm(int alarmCode);
    virtual void __fastcall RemoveAlarm(int alarmCode);
    virtual void __fastcall RemoveAlarmAll();
    virtual bool __fastcall FindAlarm(int alarmCode);
};
#endif