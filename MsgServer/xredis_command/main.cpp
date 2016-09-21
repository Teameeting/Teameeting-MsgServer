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

    ///////////////////////////////////////////////////////////
    //Test
    TestClientCmd cmd;
    cmd.test_publish();
    cmd.test_sadd();
    cmd.test_smembers();
    //Test
    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    //Work
    TestClientPublish publish;
    std::string channel = "follow_group";
    std::string groupid = "TestGroupId01";
    std::string userid1 = "TestUserId01";
    std::string userid2 = "TestUserId02";
    publish.AddUserAndPub(channel, groupid, userid1);
    publish.AddUserAndPub(channel, groupid, userid2);





    //Work
    ///////////////////////////////////////////////////////////

    return 0;
}


