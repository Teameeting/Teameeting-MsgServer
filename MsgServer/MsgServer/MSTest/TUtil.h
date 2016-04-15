#ifndef _T_UTIL_H_
#define _T_UTIL_H_

#include "TTask.h"

class TUtil
{
protected:
	TUtil(void);
public:
	virtual ~TUtil(void);

	static void Initialize(int evTbSize);

	static void DeInitialize();

	static TUtil* Inst();

public:
	int		Start();
	void	DoTick();
	void	Stop();

private:
    TTask* tt1;
    TTask* tt2;
    TTask* tt3;
};

#endif	// _T_UTIL_H_
