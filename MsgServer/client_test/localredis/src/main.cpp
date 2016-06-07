#include <iostream>
#include "RTHiredis.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, const char* argv[])
{
    long long nTimesCounter = 0;
#if 0
    RTHiredisRemote::Instance().SetHostAddr("192.168.7.213", 6379);
    RTHiredisRemote::Instance().Connect();
    std::string user("test-redis2");
    while(1)
    {
        long long seq = 0;
        RTHiredisRemote::Instance().GenericIncrId(user, &seq);
        nTimesCounter++;
    }
    RTHiredisRemote::Instance().DisConn();
#else
    RTHiredisLocal::Instance().Connect();
    std::string user("test-redis2");
    while(1)
    {
        long long seq = 0;
        RTHiredisLocal::Instance().GenericIncrId(user, &seq);
        nTimesCounter++;
    }
    RTHiredisLocal::Instance().DisConn();
#endif
    std::cout << "nTimesCounter is:" << nTimesCounter << std::endl;
    return 0;
}
