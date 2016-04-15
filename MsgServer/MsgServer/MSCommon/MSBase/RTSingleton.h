#ifndef _RT_SINGLETON_H_
#define _RT_SINGLETON_H_

#include <stdlib.h>
#include "OSMutex.h"

template<class T>
class RTSingleton {
public:
    static T& Instance() {
        if (nullptr == s_instance) {
            OSMutexLocker locker(&s_mutex);
            if (nullptr == s_instance) {
                s_instance = new T();
                atexit(Destroy);
            }
        }
        return *s_instance;
    }
protected:
    RTSingleton(){}
    virtual ~RTSingleton(){}
private:
    static void Destroy() {
        if (s_instance != nullptr) {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    static OSMutex s_mutex;
    static T* volatile s_instance;
};

template<class T>
OSMutex RTSingleton<T>::s_mutex;

template<class T>
T* volatile RTSingleton<T>::s_instance = nullptr;

#endif/*_RTSINGLETON_H_*/
