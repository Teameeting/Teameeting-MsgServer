#include <iostream>
#include <fstream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <vector>

//注意：当字符串为空时，也会返回一个空字符串
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=std::string::npos)
    {
        ret->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
    }
    if (index-last>0)
    {
        ret->push_back(s.substr(last,index-last));
    }
}

void parse_file(const char* filename, std::ofstream& ofs)
{
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    char buf[128] = {0};
    std::list<std::string> request;
    std::list<std::string> response;
    std::string requestStart("send");
    std::string responseStart("recv");
    while(ifs.good() && !ifs.eof())
    {
        memset(buf, 0, 128);
        ifs.getline(buf, 128);
        std::string str(buf);
        //std::cout << "read:" << str << std::endl;
        if (str.compare(0, 4, requestStart.c_str(), 4)==0)
        {
            request.push_back(str);
        } else if (str.compare(0, 4, responseStart.c_str(), 4)==0)
        {
            response.push_back(str);
        }
    }
    std::cout << "request.size:" << request.size() << ", response.size:" << response.size() << std::endl << std::endl;
    ifs.close();

    char result[2048] = {0};
    char sendRes[1024] = {0};
    char recvRes[1024] = {0};
    std::string strsplit(":");
    if (request.size()>0)
    {
        std::list<std::string>::iterator it = request.begin();
        std::list<std::string>::reverse_iterator rit = request.rbegin();
        std::string requestFirst = *it;
        std::string requestLast = *rit;
        std::cout << "requestFirst:"  << requestFirst  << std::endl;
        std::cout << "requestLast:"   << requestLast   << std::endl;
        std::vector<std::string>  FirstResult;
        split(requestFirst, strsplit, &FirstResult);
        for(int i=0;i < FirstResult.size();++i)
        {
            std::cout << "FirstResult i : " << i << ", "<< FirstResult[i] << std::endl;
        }
        std::vector<std::string>  LastResult;
        split(requestLast, strsplit, &LastResult);
        for(int i=0;i<LastResult.size();++i)
        {
            std::cout << "LastResult i : " << i << ", "<< LastResult[i] << std::endl;
        }
        long long timer = atoll(LastResult[1].c_str()) - atoll(FirstResult[1].c_str());
        long long counter = atoll(LastResult[3].c_str()) - atoll(FirstResult[3].c_str());
        sprintf(sendRes, "send request timer:%lld,counter:%lld,c/t:%f\n", timer, counter, (counter/(timer/1000.0)));
        std::cout << sendRes << std::endl;
    }
    if (response.size()>0)
    {
        std::list<std::string>::iterator it = response.begin();
        std::list<std::string>::reverse_iterator rit = response.rbegin();
        std::string responseFirst = *it;
        std::string responseLast = *rit;;
        std::cout << "responseFirst:" << responseFirst << std::endl;
        std::cout << "responseLast:"  << responseLast  << std::endl;

        std::vector<std::string>  FirstResult;
        split(responseFirst, strsplit, &FirstResult);
        for(int i=0;i<FirstResult.size();++i)
        {
            std::cout << "FirstResult i : " << i << ", "<< FirstResult[i] << std::endl;
        }
        std::vector<std::string>  LastResult;
        split(responseLast, strsplit, &LastResult);
        for(int i=0;i<LastResult.size();++i)
        {
            std::cout << "LastResult i : " << i << ", "<< LastResult[i] << std::endl;
        }
        long long timer = atoll(LastResult[1].c_str()) - atoll(FirstResult[1].c_str());
        long long counter = atoll(LastResult[3].c_str()) - atoll(FirstResult[3].c_str());
        sprintf(recvRes, "recv response timer:%lld,counter:%lld,c/t:%f\n", timer, counter, (counter/(timer/1000.0)));
        std::cout << recvRes << std::endl;
    }
    ofs.write(sendRes, strlen(sendRes));
    ofs.write(recvRes, strlen(recvRes));
    return;
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cout << "params is less, please add data file_path" << std::endl;
        exit(0);
    }
    std::ofstream ofs;
    ofs.open("./result", std::ofstream::out);
    parse_file(argv[1], ofs);
    ofs.close();

    return 0;
}
