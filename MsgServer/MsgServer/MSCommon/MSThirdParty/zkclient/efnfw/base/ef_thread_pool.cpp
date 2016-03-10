#include "ef_thread_pool.h"

namespace ef{

ThreadPool::ThreadPool():
	m_run(false){
	mutexInit(&m_cs);
	semInit(&m_sem, 0, 0x7FFFFFFF);
}

ThreadPool::~ThreadPool(){
	stop();
	semDestroy(&m_sem);
	mutexDestroy(&m_cs);
}

int32 ThreadPool::start(){
	m_run = true;
	threadCreate(&m_thread, NULL, threadPoolProcess, this);
	return 0;
}

void* ThreadPool::threadPoolProcess(void* vpool){
	ThreadPool* p = (ThreadPool*)vpool;

	while(true){

		Task* t = p->getTask();
		
		if(t){
			t->run();
		}else{
			break;
		}

	}

	return 0;
}


Task* ThreadPool::getTask(){

	semTake(&m_sem);

	AutoLock l(&m_cs);
	Task* t = NULL;

	int32 ret = m_tasks.read((uint8*)&t, sizeof(t));

	if(ret < (int)sizeof(t)){
		return NULL;
	}

	return t;
}

int32 ThreadPool::addTask(Task* t){

	if(!m_run){
		return -2;
	}


	AutoLock l(&m_cs);

	m_tasks.autoResizeWrite((const uint8*)&t, sizeof(t));

	semGive(&m_sem);

	return 0;
}

int32 ThreadPool::stop(){

	m_run = false;

	semGive(&m_sem);

	threadJoin(&m_thread);

	return 0;
}

}
