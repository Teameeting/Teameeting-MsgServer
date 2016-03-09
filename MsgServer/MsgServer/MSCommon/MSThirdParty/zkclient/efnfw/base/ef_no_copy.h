#ifndef __EF_NO_COPY_H__
#define __EF_NO_COPY_H__

namespace ef{

/// base class to disable copy and asign construct
class NoCopy
{
protected:
	NoCopy() {}
	virtual ~NoCopy() {}
private:
	NoCopy(const NoCopy &);
	NoCopy &operator=(const NoCopy &);
};

};

#endif

