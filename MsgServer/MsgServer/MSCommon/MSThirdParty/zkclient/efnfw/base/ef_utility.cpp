#include "ef_utility.h"
#include <sstream>
#if defined(GPUSH_LINUX)
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cwctype>

namespace ef{


using namespace std;

	void trim(string& s) {
		if (s.empty()) {
			return;
		}

		string::iterator c;
		// Erase whitespace before the string
		for (c = s.begin(); c != s.end() && iswspace(*c++););
		s.erase(s.begin(), --c);

		// Erase whitespace after the string
		for (c = s.end(); c != s.begin() && iswspace(*--c););
		s.erase(++c, s.end());
	}

	string itostr(int64 i){
		stringstream os;
		os << i;
		return os.str();
	}

	int64 gpush_htonll(int64 ll){
		int64 ret = ll;
		char* p = (char*)&ret;
		for(int i = 0; i < 4; ++i){
			char c = p[i];
			p[i] =  p[7 - i];
			p[7 - i] = c;
		}
		return	ret;
	}

	int64 tv_diff_ms(struct timeval t1, struct timeval t2){
		return (t1.tv_sec - t2.tv_sec) * 1000 + (t1.tv_usec - t2.tv_usec) / 1000;
	//	if(t1.tv_sec < t2.tv_sec){
	//		return	-1;
	//	}else	if(t1.tv_sec > t2.tv_sec){
	//		return	1;
	//	}else{
	//		if(t1.tv_usec < t2.tv_usec){
	//			return	-1;
	//		}else	if(t1.tv_usec > t2.tv_usec){
	//			return	1;
	//		}else{
	//			return	0;
	//		}
	//	}
	}

	struct timeval tv_diff(struct timeval t1, struct timeval t2){
		struct	timeval	ret;

		ret.tv_sec = t1.tv_sec - t2.tv_sec;

		if(t1.tv_usec < t2.tv_usec){
			ret.tv_sec -= 1;
			t1.tv_usec += 1000000;
		}

		ret.tv_usec = t1.tv_usec - t2.tv_usec;

		return	ret;
	}


	int64 gettime_ms(){
		struct timeval tp;
		gettimeofday(&tp, NULL);
		return tp.tv_sec * 1000 + tp.tv_usec / 1000;
	}

	int split(const string& str, vector<string>& ret_, 
		string sep)
	{
	    if (str.empty())
	    {
		return 0;
	    }

	    string tmp;
	    string::size_type pos_begin = str.find_first_not_of(sep);
	    string::size_type comma_pos = 0;

	    while (pos_begin != string::npos)
	    {
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != string::npos)
		{
		    tmp = str.substr(pos_begin, comma_pos - pos_begin);
		    pos_begin = comma_pos + sep.length();
		}
		else
		{
		    tmp = str.substr(pos_begin);
		    pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
		    ret_.push_back(tmp);
		    tmp.clear();
		}
	    }
	    return 0;
	}

	int32 sleep_ms (int32 msec)
	{
		int32  ret = 0; 
#if defined(GPUSH_LINUX)
		struct epoll_event ev[1];
		int32 fd = epoll_create(1);
		ret = epoll_wait(fd, ev, 1, msec);
		close(fd);
		if(ret < 0){
			printf("sleep_ms err:%s\n", strerror(errno));
		}
#else
        printf("my implement sleep_ms usleep(msec*1000)!!!\n");
        usleep(msec*1000);
#endif

		return ret;
	}
};

