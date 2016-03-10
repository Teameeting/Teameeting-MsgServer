#ifndef EF_AUTO_PTR_H
#define EF_AUTO_PTR_H

#include "ef_atomic.h"

namespace ef{

template<class T>
struct Deleter{
	void operator()(T* t){
		delete t;
	}
};

template<class T>
struct ArrayDeleter{
	void operator()(T* t){
		delete [] t;
	}
};

template<class T, class D>
class RefCntPtrImp{
public:
	explicit RefCntPtrImp(T* ptr)
		:m_ptr(ptr),
		m_cnt(1){
	}
	
	~RefCntPtrImp(){
		if(m_ptr)
			m_deleter(m_ptr);	
	}

	int32 incRef(){
		return atomicIncrement32(&m_cnt);
	}

	int32 decRef(){
		int32 cnt = atomicDecrement32(&m_cnt);
		if(cnt <= 0)
			delete this;
		return cnt;
	}


	T* get(){
		return m_ptr;
	}

private:
	T* m_ptr;
	D m_deleter;
	volatile int32 m_cnt;
};

template<class T>
class RefCntPtr{
public:
	explicit RefCntPtr(T* t = NULL):
		m_imp(new RefCntPtrImp<T, Deleter<T> >(t)){
	}

	~RefCntPtr(){
		m_imp->decRef();
	}

	RefCntPtr(const RefCntPtr& ptr){
		ptr.m_imp->incRef();
		m_imp = ptr.m_imp;
	}

	RefCntPtr& operator = (RefCntPtr& ptr){
		m_imp->decRef();
		ptr.m_imp->incRef();
		m_imp = ptr.m_imp;
		return *this;
	}

	T* operator->(){
		return get();
	}

	T* get(){
		return m_imp->get();
	}

	void reset(T* t){
		RefCntPtrImp<T, Deleter<T> >* imp 
			= new RefCntPtrImp<T, Deleter<T> >(t);
		m_imp->decRef();
		m_imp = imp;
	}

private:
	mutable RefCntPtrImp<T, Deleter<T> >* m_imp;
};

template<class T>
class ArrayRefCntPtr{
public:
	explicit ArrayRefCntPtr(T* t = NULL):
		m_imp(new RefCntPtrImp<T, ArrayDeleter<T> >(t)){
	}

	~ArrayRefCntPtr(){
		m_imp->decRef();
	}

	ArrayRefCntPtr(const ArrayRefCntPtr& ptr){
		ptr.m_imp->incRef();
		m_imp = ptr.m_imp;
	}

	ArrayRefCntPtr& operator = (ArrayRefCntPtr& ptr){
		m_imp->decRef();
		ptr.m_imp->incRef();
		m_imp = ptr.m_imp;
		return *this;
	}

	T* operator->(){
		return get();
	}

	T& operator[](size_t idx){
		return get()[idx];
	}

	T* get(){
		return m_imp->get();
	}

	void reset(T* t){
		RefCntPtrImp<T, ArrayDeleter<T> >* imp 
			= new RefCntPtrImp<T, ArrayDeleter<T> >(t);
		m_imp->decRef();
		m_imp = imp;
	}

private:
	mutable RefCntPtrImp<T, ArrayDeleter<T> >* m_imp;
};



};


#endif
