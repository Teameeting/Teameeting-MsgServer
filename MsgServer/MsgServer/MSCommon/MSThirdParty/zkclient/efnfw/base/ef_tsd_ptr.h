#ifndef __EF_TSD_PTR_H__
#define __EF_TSD_PTR_H__

#include <stdlib.h>
#include <pthread.h>

namespace ef{

/// deleter: do nothing
inline void nullDeleter(void *ptr) {}

/// deleter: use free ptr
inline void mallocDeleter(void *ptr) { if (ptr) { free(ptr); } }

/// deleter: use delete ptr
template<typename T>
void newDeleter(void *ptr) { delete(T *)ptr; }

/// deleter: use delete [] ptr;
template<typename T>
void newArrayDeleter(void *ptr) { delete [](T *)ptr; }

/// define deleter function type
typedef void(*DeleterCB)(void *);

/// Thread Specific Storage
/// @param T pointer type tss stored
template<typename T>
class TSDPtr
{
	pthread_key_t m_pkey;
	DeleterCB m_d;

	TSDPtr(const TSDPtr &);
	TSDPtr &operator=(const TSDPtr &);
public:
	/// @param d deleter, NewDeleter<T>, 
	/// NewArrayDeleter<T>, MallocDeleter, NullDeleter
	explicit TSDPtr(DeleterCB d = newDeleter<T>) 
		: m_d(d) { pthread_key_create(&m_pkey, d); }
	~TSDPtr() {
		T *tmp = release();

		if (tmp) { m_d(tmp); }

		pthread_key_delete(m_pkey);
	}

	inline T *get() { return (T *)pthread_getspecific(m_pkey); }
	inline bool set(T *ptr) { return 0 == pthread_setspecific(m_pkey, ptr);}

	inline T *operator -> () { return get(); }
	inline T &operator * () { return *get(); }

	inline bool operator !() { return 0 == get(); }
	inline operator bool() { return 0 != get(); }

	T *release() {
		T *tmp = get();
		set(0);
		return tmp;
	}

	void reset(T *ptr) {
		T *tmp = get();

		if (tmp != ptr) {
			set(ptr);
			m_d(tmp);
		}
	}
};

};

#endif

