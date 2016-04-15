#ifndef	__EF_LOOP_BUF_H__
#define	__EF_LOOP_BUF_H__

#include"ef_btype.h"

namespace	ef{

struct frame{
	uint8   *m_buf;
	int32   m_cap;
	int32   m_size;
	int32   m_start;
	frame	*m_next;
	int32   read(uint8 *buf, int32 len);
	int32	write(const uint8 *buf, int32 len);

	int32	peek(uint8 *buf, int32 len) const;

	int32	freeSize() const{
		return m_cap - m_size - m_start;
	}

	int32	full() const{
		return m_size + m_start == m_cap;
	}

	int32   clear();
};

class	LoopBuf{
	public:

		enum{
			DEFAUTLT_ALIGN_SIZE = 2048,
		};

		LoopBuf();
		~LoopBuf();

		int32	capacity() const;
		int32	extend(int32 sz);
		//if buf is null, just drop data
		int32	read(uint8 *buf, int32 len);
		//if buf is null, just inc size
		int32	write(const uint8 *buf, int32 len);
		int32	autoResizeWrite(const uint8 *buf, int32 len);
		int32	peek(uint8 *buf, int32 len) const;
		int32	firstFrameLen() const;
		const uint8* firstFrameData() const;
		int32	freeFrameLen() const;
		uint8*	freeFrameBuf();	
		int32	size() const{
			return m_size;
		}
	
		int32	find(uint8 c) const;
		int32	clear();

	private:

		int32	alignSize(int32 len);
		frame*	constructFrame(uint8* buf, int32 len);
		frame*	allocFrame(int32 len);

		frame*	m_head;
		frame*	m_w_pos;
		frame*	m_tail;	
		int32	m_size;
};

};


#endif/**/
