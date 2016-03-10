#ifndef __EF_STSTISTIC_H__
#define __EF_STSTISTIC_H__

#include <string>
#include <sys/time.h>

namespace ef{

enum{
	//10s out put 
	DEFAULT_OUT_SPAN = 10 * 1000000,
};

class TimeRecorder{
public:
	TimeRecorder(const std::string& op);
	~TimeRecorder();
private:
	std::string m_op;
	timeval m_start;
};

typedef int (*OUT_FUNC)(void* par, const std::string& l);

int initStatistic(OUT_FUNC, void* par, int outspan = DEFAULT_OUT_SPAN);
int uninitStatistic();

};

#endif/*EF_STSTISTIC_H*/
