#include <iostream>
#include <string>

extern "C"{
#include <assert.h>
}

#include "RCPthread.h"

RCPthread::RCPthread(void* data)
: _p_data(data)
, _p_thread(nullptr)
{

}

RCPthread::~RCPthread()
{

}

void RCPthread::_Init()
{
    std::cout << "Start RCPthread..." << std::endl;
    _p_thread = new STPthread();
    assert(_p_thread);
    _p_thread->_self = this;
    _p_thread->_data = _p_data;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(pthread_create(&_m_pid, &attr, (void*(*)(void*))RCPthread::_Start, (void*)_p_thread) != 0)
    {
        std::cout << "pthread_create failed" << std::endl;
        assert(false);
    }
    pthread_attr_destroy(&attr);
}

void RCPthread::_Unin()
{
    if (_p_thread)
    {
         delete _p_thread;
         _p_thread = nullptr;
    }
    _p_data = nullptr;
    std::cout << "Del STPthread..." << std::endl;
}

void RCPthread::_Start(void* p)
{
    std::cout << "Run RCPthread...:" << std::endl;
    STPthread* pst = (STPthread*)p;
    ((RCPthread*)(pst->_self))->Run(pst->_data);
}
