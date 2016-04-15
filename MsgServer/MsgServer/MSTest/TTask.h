#ifndef _T_TASK_H_
#define _T_TASK_H_

#include "Task.h"
#include "TimeoutTask.h"

class TTask : public Task{
public:
    TTask();
    virtual ~TTask();

    void SetTimer(int time) {Assert(time>0);fTimeoutTask.SetTimeout(time);}
    void SetTickTimer(int time) {Assert(time>0);fTickTime = time;}
    void UpdateTimer() {fTimeoutTask.RefreshTimeout();}

protected:
    virtual SInt64 Run();
private:
    TimeoutTask         fTimeoutTask;
    UInt32              fTickTime;
};


#endif // _T_TASK_H_
