#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    if (argc < 2)
        return 0;
    //std::cout << "user name:" << argv[1] << std::endl;
    printf("user name:%s\n", argv[1]);
    while(1)
        sleep(1);
    //return 0;
    exit(0);
}
