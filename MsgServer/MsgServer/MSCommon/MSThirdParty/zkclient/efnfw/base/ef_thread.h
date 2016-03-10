/*
 * Copyright (c) 2010
 * efgod@126.com.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */


#ifndef __EF_THREAD_H__
#define __EF_THREAD_H__



#ifdef _WIN32

#include <windows.h>


#else/*_WIN32*/

#include <pthread.h>
#include <semaphore.h>


#endif

#include "ef_btype.h"

namespace ef{

#ifdef _WIN32
	typedef HANDLE   THREADHANDLE;
	typedef DWORD   PID;
	typedef CRITICAL_SECTION MUTEX;
	typedef HANDLE   SEMAPHORE;
	typedef LPTHREAD_START_ROUTINE PTHREAD_FUNC;


#else
	typedef pthread_t  THREADHANDLE;
	typedef pid_t   PID;
	typedef pthread_mutex_t  MUTEX;
	typedef sem_t   SEMAPHORE;
	typedef void* (*PTHREAD_FUNC)(void*);

#endif

	int32 threadCreate(THREADHANDLE* thd, void* attr, PTHREAD_FUNC start_routine, void* arg);
	int32 threadJoin (THREADHANDLE* thd);
#ifdef _WIN32
	int32 threadSuspend(THREADHANDLE* thd);
	int32 threadResume(THREADHANDLE* thd);
#endif
	int32 mutexInit (MUTEX* mtx);
	int32 mutexTake (MUTEX* mtx);
	int32 mutexGive (MUTEX* mtx);
	int32 mutexDestroy(MUTEX* mtx);
	int32 semInit (SEMAPHORE* sem, int32 initcnt, int32 maxcnt);
	int32 semTake (SEMAPHORE* sem, bool couldbreak = false);
	int32 semTryTake (SEMAPHORE* sem);
	int32 semGive (SEMAPHORE* sem);
	int32 semDestroy (SEMAPHORE* sem);

	class AutoLock
	{
		public:
			AutoLock(MUTEX* lock):m_lock(lock){
				mutexTake(m_lock);
			}
			~AutoLock( ){
				mutexGive(m_lock);
			}
		protected:
		private:
			MUTEX* m_lock;
	};

};

#endif/*BE_THREAD_H*/
