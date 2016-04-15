#ifndef __EF_SINGLETON_H__
#define __EF_SINGLETON_H__

#include <pthread.h>
#include <stdlib.h>

#include "ef_no_copy.h"

namespace ef{

template<typename T>
class Singleton :
	NoCopy
{
public:
	static T *instance() {
		pthread_once(&m_ponce, init);
		return m_ptr;
	}

private:
	Singleton();
	~Singleton();

	static void init() {
		m_ptr = new T();
		::atexit(destroy);
	}

	static void destroy() {
		delete m_ptr;
	}

private:
	static pthread_once_t m_ponce;
	static T *m_ptr;
};

template<typename T>
pthread_once_t Singleton<T>::m_ponce = PTHREAD_ONCE_INIT;

template<typename T>
T *Singleton<T>::m_ptr = NULL;

};

#endif

