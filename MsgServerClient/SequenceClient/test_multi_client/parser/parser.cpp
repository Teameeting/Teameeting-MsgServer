#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cout << "params is less, please add data file_path" << std::endl;
        exit(0);
    }

    FILE* pfile = fopen(argv[1], "r");
    if (!pfile)
    {
        std::cout << "file argv[1]:" << argv[1] << " is not exists, please check" << std::endl;
        exit(0);
    }
    char buf[128] = {0};
    std::list<int> seqns;
    while(fread(buf, 1, 128, pfile))
    {
        std::string str(buf);
        std::size_t found = str.find_last_of(":");
        if (found!=std::string::npos)
        {
            seqns.push_back(atoi(str.substr(found+1).c_str()));
        }
        if (feof(pfile) || ferror(pfile))
        {
            std::cout << "read file meet feof or ferror" << std::endl;
            break;
        }
    }
    fclose(pfile);
    pfile = NULL;
    std::list<int>::iterator it = seqns.begin();
    int current = *it;
    bool success = true;
    while((++it)!=seqns.end())
    {
        if ((current+1) == (*it))
        {
            printf("current:%d, next:%d\n", current, *it);
            current = *it;
            continue;
        }
        else
        {
            success = false;
            break;
        }
    }

    struct timeval nows;
    gettimeofday(&nows, NULL);
    time_t nowd;
    time(&nowd);
    struct tm* timenow;
    timenow = localtime(&nowd);


    std::string errlog(argv[1]);
    errlog.append("-log");
    char err[128] = {0};
    if (success)
    {
        sprintf(err, "success,date:%s[] time:%lld, current seqn:%d, next seqn:%d\n", asctime(timenow), (long long)nows.tv_sec, current, *it);
    } else {
        sprintf(err, "error,date:%s[] time:%lld, current seqn:%d, next seqn:%d\n", asctime(timenow), (long long)nows.tv_sec, current, *it);
    }
    FILE* f = fopen(errlog.c_str(), "a");
    fwrite(err, 1, 128, f);
    fclose(f);
    f = NULL;
    return 0;
}
