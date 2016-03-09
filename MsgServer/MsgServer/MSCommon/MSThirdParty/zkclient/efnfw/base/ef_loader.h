#ifndef __EF_LOADER_H__
#define __EF_LOADER_H__

#include "ef_thread.h"

namespace ef{

template<class T>
class DataSrc{
public:
	DataSrc():m_version(0){
		mutexInit(&m_cs);
	}

	~DataSrc(){
		mutexDestroy(&m_cs);
	}

	int setData(const T& t){
		AutoLock l(&m_cs);
		m_data = t;
		++m_version;
		return m_version;
	}

	int getSersion() const{
		return m_version;
	}

	void getData(T& t){
		AutoLock l(&m_cs);
		t = m_data;
	}

private:
	MUTEX m_cs;
	T m_data;
	volatile int m_version;
};

template<class T>
class Loader{
public:
	Loader(DataSrc<T>* s = NULL):m_src(s), m_version(0){
	}

	void setSrc(DataSrc<T>* s){
		m_version = 0;
		m_src = s;
	}

	bool loadData(){
		int v = m_src->getSersion();
		if(m_version < v){
			m_src->getData(m_data);
			m_version = v;
			return true;
		}

		return false;
	}


	const T& getData() const{
		return m_data;
	}

private:
	DataSrc<T>* m_src;
	T m_data;
	volatile int m_version;
};

}

#endif
