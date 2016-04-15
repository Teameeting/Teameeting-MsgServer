/*
 * Copyright (c) 2010
 * efgod@126.com.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  efgod@126.com. makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */
#ifndef __EF_ATOMIC_H__
#define __EF_ATOMIC_H__


#include "ef_btype.h"

namespace ef{

	int64  atomicCompareExchange64 (volatile int64* dst,
			int64 val, int64 cmp);
	bool atomicCompareExchange64_1 (volatile int64* dst, 
			int64 val, int64 cmp);
	int64  atomicExchange64   (volatile int64* dst, 
			int64 val);
	int64  atomicAdd64    (volatile int64  * dst, 
			int64 val);
	//The function returns the resulting incremented value.
	int64  atomicIncrement64   (volatile int64  * dst);
	//The function returns the resulting decremented value.
	int64  atomicDecrement64   (volatile int64  * dst);
	int32  atomicCompareExchange32 (volatile int32* dst, int32 val, 
			int32 cmp);
	bool atomicCompareExchange32_1 (volatile int32* dst, int32 val, 
			int32 cmp);
	int32  atomicExchange32   (volatile int32* dst, int32 val);
	int32  atomicAdd32    (volatile int32* dst, int32 val);
	//The function returns the resulting incremented value.
	int32  atomicIncrement32   (volatile int32* dst);
	//The function returns the resulting decremented value.
	int32  atomicDecrement32   (volatile int32* dst);

	void* atomicCompareExchangePtr (volatile void** dst,
			void* val, void* cmp);
	void* atomicExchangePtr   (volatile void** dst, 
			void* val);

	int32  getCpuCore();

	int32  atomic_test();

};



#endif/*BE_ATOMIC_H*/

