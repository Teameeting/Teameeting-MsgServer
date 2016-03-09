#ifndef	__EF_UTILITY_H__
#define __EF_UTILITY_H__

#include "ef_btype.h"
#include <sys/time.h>
#include <time.h>
#include <string>
#include <vector>

namespace ef{

	int64 gettime_ms();

	int64 tv_diff_ms(struct timeval t1, struct timeval t2);
	struct timeval tv_diff(struct timeval t1, struct timeval t2);

	int64 gpush_htonll(int64 l);

	int split(const std::string& str, std::vector<std::string>& ret_, 
		std::string sep = ",");

	std::string itostr(int64 i);

	void trim(std::string& str);

	int32 sleep_ms (int32 msec);
};

#endif/*EF_UTILITY_H*/

