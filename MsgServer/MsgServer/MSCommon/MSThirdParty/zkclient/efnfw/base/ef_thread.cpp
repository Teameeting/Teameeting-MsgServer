#include "ef_thread.h"
#include <errno.h>

namespace ef{


	int32 threadCreate(THREADHANDLE* thread, void* attr, 
			PTHREAD_FUNC start_routine, void* arg)
	{
		int32  ret = 0;
#ifdef _WIN32
		*thread  = CreateThread(NULL,0, start_routine, arg, 0, NULL);
		if(false == *thread) return -1;
#else
		ret   = pthread_create(thread, NULL,start_routine, arg);
#endif
		return ret;
	}

	int32 threadJoin (THREADHANDLE* thread)
	{
		int32  ret = 0;
#ifdef _WIN32
		DWORD dwret;
		dwret = WaitForSingleObject(*thread, INFINITE);
		if(dwret != WAIT_OBJECT_0) ret = -1;
#else
		ret  = pthread_join(*thread, NULL);
#endif
		return ret;
	}

#ifdef _WIN32
	int32 threadSuspend (THREADHANDLE* thread)
	{
		int32 ret = 0;
		ret  = SuspendThread(*thread);
		if(ret < 0) ret = -1;
		else   ret = 0;
		return ret;
	}
	int32 threadResume (THREADHANDLE* thread)
	{
		int32 ret = 0;
		ret  = ResumeThread(*thread);
		if(ret < 0) ret = -1;
		else   ret = 0;
		return ret;
	}
#endif

	int32 mutexInit (MUTEX*   mtx)
	{
		int32  ret = 0;
#ifdef _WIN32
		InitializeCriticalSection(mtx);
#else
		ret  = pthread_mutex_init(mtx, NULL);
#endif
		return ret;
	}
	int32 mutexTake (MUTEX*   mtx)
	{
		int32  ret = 0;
#ifdef _WIN32
		EnterCriticalSection(mtx);
#else
		ret  = pthread_mutex_lock(mtx);
#endif
		return ret;
	}
	int32 mutexGive (MUTEX*   mtx)
	{
		int32  ret = 0;
#ifdef _WIN32
		LeaveCriticalSection(mtx);
#else
		ret  = pthread_mutex_unlock(mtx);
#endif
		return ret;
	}

	int32 mutexDestroy(MUTEX*   mtx)
	{
		int32  ret = 0;
#ifdef _WIN32
		DeleteCriticalSection(mtx);
#else
		ret  = pthread_mutex_destroy(mtx);
#endif
		return ret;
	}


	int32 semInit (SEMAPHORE* sem, int32 initcnt, int32 maxcnt)
	{
		int32  ret = 0;
#ifdef _WIN32

		*sem = CreateSemaphore(NULL, initcnt, maxcnt, NULL);
		if(false == *sem) return -1;
#else
		ret  = ::sem_init(sem, 0, initcnt);
#endif
		return ret;
	}
	int32 semTake (SEMAPHORE* sem, bool couldbreak)
	{
		int32  ret = 0;
#ifdef _WIN32
		DWORD dwret;
		dwret = WaitForSingleObject(*sem, INFINITE);
		if(dwret != WAIT_OBJECT_0) ret = -1;
#else
loop:
		ret = ::sem_wait(sem);
		if(ret < 0 && errno == EINTR && !couldbreak){
			goto loop;
		}
#endif
		return ret;
	}

	int32 semTryTake (SEMAPHORE* sem)
	{
		int32  ret = 0;
#ifdef _WIN32
		DWORD dwret;
		dwret = WaitForSingleObject(*sem, 0);
		if(dwret != WAIT_OBJECT_0) ret = -1;
#else
		//loop:
		ret = ::sem_trywait(sem);
		//if(ret < 0 && errno == EINTR){
		// goto loop;
		//}
#endif
		return ret;
	}

	int32 semGive  (SEMAPHORE*  sem)
	{
		int32  ret = 0;
#ifdef _WIN32
		if(!ReleaseSemaphore(*sem, 1, NULL))
			ret = -1;
#else
loop:
		ret = ::sem_post(sem);
		if(ret < 0 && errno == EINTR){
			goto loop;
		}
#endif
		return ret;
	}

	int32 semDestroy (SEMAPHORE*  sem)
	{
		int32  ret = 0;
#ifdef _WIN32
		if(!CloseHandle(*sem)) ret = -1;
#else
		ret = ::sem_destroy(sem);
#endif
		return ret;
	}

};
