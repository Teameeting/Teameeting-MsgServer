#ifndef __EF_THREAD_POOL_H__
#define __EF_THREAD_POOL_H__

#include "ef_thread.h"
#include "ef_loop_buf.h"

namespace ef{

class Task{
public:
	virtual ~Task(){};
	virtual int32 run() = 0;
};

class ThreadPool{
public:
	ThreadPool();
	~ThreadPool();

	int32 start();
	int32 stop();

	int32 addTask(Task* t);
private:
	//if return NULL, thread pool exit
	Task* getTask();
	static void* threadPoolProcess(void* vpool);

	int32 processOneTask();	

	int32 tryProcessAllTask();

	bool m_run;
	THREADHANDLE m_thread;
	MUTEX m_cs;
	SEMAPHORE m_sem;
	LoopBuf m_tasks;
};

};

#endif
