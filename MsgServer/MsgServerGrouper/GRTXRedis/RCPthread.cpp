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
    if(pthread_create(&_m_pid, 0, (void*(*)(void*))RCPthread::_Start, (void*)_p_thread) != 0)
    {
        std::cout << "pthread_create failed" << std::endl;
        assert(false);
    }
}

void RCPthread::_Unin()
{
    std::cout << "Join RCPthread..." << std::endl;
    pthread_join(_m_pid, NULL);
    std::cout << "Stop RCPthread..." << std::endl;
    if (_p_thread)
    {
         delete _p_thread;
         _p_thread = nullptr;
    }
    std::cout << "Del STPthread..." << std::endl;
}

void RCPthread::_Start(void* p)
{
    std::cout << "Run RCPthread...:" << std::endl;
    STPthread* pst = (STPthread*)p;
    ((RCPthread*)(pst->_self))->Run(pst->_data);
}
