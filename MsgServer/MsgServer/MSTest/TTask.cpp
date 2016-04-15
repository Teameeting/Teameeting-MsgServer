#include "TTask.h"

TTask::TTask()
    : Task()
    , fTimeoutTask(NULL, 5*1000)
    , fTickTime(0)
{
    printf("TTask::TTask was called\n");
    fTimeoutTask.SetTask(this);
}

TTask::~TTask()
{
    printf("TTask::~TTask was called\n");
}

SInt64 TTask::Run()
{
    EventFlags events = this->GetEvents();
    this->ForceSameThread();

    if (events&Task::kTimeoutEvent) {
        printf("TTask::Run timeoutevent\n");
        UpdateTimer();
        return 0;
    }
    if (events&Task::kKillEvent) {
        printf("TTask::Run killevent\n");
        return -1;
    }
    printf("TTask::Run other event return 0\n");
    return 0;

}
