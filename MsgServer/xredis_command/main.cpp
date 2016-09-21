#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

#include "test_client_cmd.h"
#include "test_client_publish.h"

int main(int argc, char **argv)
{
    printf("%d %s\r\n", argc, argv[0]);
    std::cout << "argc:" << argc << ", argv0:" << argv[0] << std::endl;

    if (argc < 1)
    {
        printf("please add params num\n");
        return 0;
    }
    std::cout << "the number is:" << argv[1] << std::endl;

    ///////////////////////////////////////////////////////////
    //Test
    //TestClientCmd cmd;
    //cmd.test_publish();
    //cmd.test_sadd();
    //cmd.test_smembers();
    //Test
    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    //Work

    int num = std::atoi(argv[1]);
    TestClientPublish publish;
    std::string channel = "follow_group";
    std::string groupid = "TestGroupId01";

    for (int i=1;i<num;i++)
    {
        std::string si = std::to_string(i);
        std::string userid = "TestUserId0";
        userid.append(si);
        std::cout << "the user is:" << userid << std::endl;
        publish.AddUserAndPub(channel, groupid, userid);
    }



    //Work
    ///////////////////////////////////////////////////////////

    return 0;
}


