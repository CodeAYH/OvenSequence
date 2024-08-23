#include <System.DateUtils.hpp>

#pragma hdrstop

#include "Defines.h"
#include "BaseThread.h"
#include "HelpFunctions.h"

#pragma package(smart_init)

__fastcall BaseThread::BaseThread(int moduleType)
    : TThread(false), FModuleType(moduleType)
{
    this->FStep = -1;
    this->FEnabled = true;
}

__fastcall BaseThread::~BaseThread()
{
}

void __fastcall BaseThread::Execute()
{
    while (!Terminate)
    {
        if (this->FEnabled == true)
        {
            DoWorkSequence();
        }
        WaitForSingleObject((void *)Handle, THREAD_WAIT_TIME);
    }
}

void __fastcall BaseThread::LogStepChanged()
{
    if (this->StepChanged)
    {
        StepChanged = false;

        String str;
        str.sprintf(L"Step = %s", this->GetStepString().c_str());
        AddSequenceLog(this->ModuleName, str, this->ModuleType);
    }
}

int __fastcall BaseThread::GetRunStatus()
{
    return this->FRunStatus;
}

void __fastcall BaseThread::SetRunStatus(int status)
{
    this->FRunStatus = status;
}

void __fastcall BaseThread::ResumeRunStatus()
{
    if (this->PrevRunStatus > 0)
    {
        this->RunStatus = this->PrevRunStatus;
    }
}

void __fastcall BaseThread::SetStep(int step)
{
    if (FStep != step)
    {
        FStep = step;
        StepChanged = true;
        LogStepChanged();
    }
}

void __fastcall BaseThread::Initialize()
{
    Initialize();
}

String __fastcall BaseThread::GetStepString()
{
    String str;
    int count = FStepString.size();
    if (FStep >= 0 && FStep < count)
    {
        str = FStepString[FStep];
    }
    else
    {
        str = L"STEP_NULL";
    }
    return str;
}

void __fastcall BaseThread::DelayTime(unsined int milliSeconds, bool workInThread = true)
{
    TDateTime now = Now();
    while (true)
    {
        if (workInThread)
        {
            WaitForSingleObject((void *)Handle, THREAD_WAIT_TIME);
        }
        else
        {
            Application->ProcessMessages();
            Sleep(THREAD_WAIT_TIME);
        }

        if (MilliSecondsBetween(now, Now()) >= milliSeconds)
        {
            break;
        }
    }
}

bool __fastcall BaseThread::FindAlarm(int alarmCode)
{
    auto it = find(AlarmList.begin(), AlarmList.end(), alarmCode);
    return (it != AlarmList.end());
}

void __fastcall BaseThread::AddAlarm(int alarmCode)
{
    if (FindAlarm(alarmCode) == false)
    {
        AlarmList.push_back(alarmCode);
    }
}

void __fastcall BaseThread::RemoveAlarm(int alarmCode)
{
    auto it = find(AlarmList.begin(), AlarmList.end(), alarmCode);
    if (it != AlarmList.end())
    {
        AlarmList.erase(it);
    }
}

void __fastcall BaseThread::RemoveAlarmAll()
{
    AlarmList.clear();
}