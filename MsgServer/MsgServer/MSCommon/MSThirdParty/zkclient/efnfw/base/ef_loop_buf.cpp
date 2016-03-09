#include "ef_loop_buf.h"
#include <cassert>
#include <string.h>

namespace	ef{

	int32	frame::peek(uint8 *buf, int32 len) const{
		int32	realread = len;

		if(m_size <= len){
			realread = m_size;
		}

		if(buf){
			memmove(buf, m_buf + m_start, realread);
		}

		return realread;

	}

	int32	frame::read(uint8 *buf, int32 len){
		int32	realread = peek(buf, len);
		m_start += realread;
		m_size -= realread;					

		if(m_size <= 0){
			m_start = 0;
		}

		return realread;
	}

	int32	frame::write(const uint8 *buf, int32 len){
		int32	realwrite = len;

		if(realwrite > freeSize()){
			realwrite = freeSize();
		}

		int32 stop = m_start + m_size;

		if(buf){
			memmove(m_buf + stop, buf, realwrite);
		}

		m_size += realwrite;
		return realwrite;
	}


	int32	frame::clear(){
		int32 ret = m_size;
		m_size = 0;
		m_start = 0;
		return ret;
	}

	int32	LoopBuf::firstFrameLen() const{
		assert(m_head);
		return m_head->m_size;
	}

	const uint8* LoopBuf::firstFrameData() const{
		assert(m_head);
		return m_head->m_buf + m_head->m_start;
	}

	int32	LoopBuf::freeFrameLen() const{
		assert(m_w_pos);
		return m_w_pos->m_cap - m_w_pos->m_size - m_w_pos->m_start;	
	}

	uint8*	LoopBuf::freeFrameBuf(){
		assert(m_w_pos);
		return m_w_pos->m_buf + m_w_pos->m_start + m_w_pos->m_size; 
	}

	int32	LoopBuf::write(const uint8 *buf, int32 len){
		int32 wlen = 0;
		int32 ret = 0;
		frame*	f = m_w_pos;
		while(len > wlen && f && !f->full()){
			ret = f->write(buf + wlen, len - wlen);
			if(f->full() && f->m_next){
				f = f->m_next;
				m_w_pos = f;
			}

			wlen += ret;
		}
		m_size += wlen;
		return wlen;
	}

	int32	LoopBuf::autoResizeWrite(const uint8 *buf, int32 len){
		int32 ret = write(buf, len);
		if(len > ret){
			extend(len - ret);
			write(buf + ret, len -ret);
		}
		return len;
	}

	int32	LoopBuf::peek(uint8 *buf, int32 len) const{
		int32 rlen = 0;
		int32 ret = 0;
		frame*  f = m_head;
		frame*	n = NULL;
		while(len > rlen && f && f != m_w_pos->m_next){
			n = f->m_next;
			ret = f->peek(buf + rlen, len - rlen);	
			rlen += ret;
			f = n;
		}	
		return rlen;
	}

	int32	LoopBuf::read(uint8 *buf, int32 len){
		int32 rlen = 0;
		int32 ret = 0;
		frame*  f = m_head;
		frame*  n = NULL;
		while(len > rlen && f && f != m_w_pos->m_next){
			n = f->m_next;
			ret = f->read(buf + rlen, len - rlen);	
			rlen += ret;
			if(!f->m_size && m_tail != f){
				m_head = n;
				m_tail->m_next = f;
				m_tail = f;
				f->m_next = NULL;
			}
			f = n;
		}	
		m_size -= rlen;
		return rlen;

	}

	int32	LoopBuf::find(uint8 c) const{
		int32 ret = 0;
		frame*  f = m_head;
		frame*  n = NULL;
		while(f){
			n = f->m_next;
			for(int32 i = 0; i < f->m_size; ++i){
				if(c == f->m_buf[f->m_start + i]){
					ret += i;
					return ret;
				}
			}	
			ret += f->m_size;
			f = n;
		}	
		if(!f){
			return -1;
		}
		return ret;

	}


	LoopBuf::LoopBuf(){
		frame* f = allocFrame(1024); 
		m_head = f;
		m_w_pos = f;
		m_tail = f;
		m_size = 0;
	}

	frame*	LoopBuf::constructFrame(uint8* buf, int32 len){
		frame* f = (frame*)buf;
		f->m_size = 0;
		f->m_start = 0;
		f->m_next = NULL;	
		f->m_cap = len - sizeof(frame);
		f->m_buf = buf + sizeof(frame);
		return f;	
	}


	int32	LoopBuf::alignSize(int32 l){
		return l + DEFAUTLT_ALIGN_SIZE - l % DEFAUTLT_ALIGN_SIZE;
	}

	frame*	LoopBuf::allocFrame(int32 l){
		int32 len = alignSize(l);
		uint8* buf = new uint8[len];
		frame* f = constructFrame(buf, len);
		return f;
	}	


	int32	LoopBuf::extend(int32 sz){
		frame* f = allocFrame(sz);
		m_tail->m_next = f;
		m_tail = f;
		if(m_w_pos->full())
			m_w_pos = m_w_pos->m_next;
		return f->m_cap;
	}	

	int32	LoopBuf::clear(){
		frame* f = m_head;
		for(; f; f = f->m_next)	
			f->clear();
		int32 ret = m_size;
		m_size = 0;
		return ret;
	}

	LoopBuf::~LoopBuf(){
		frame* f = m_head;
		frame* n = NULL;
		for(; f; f = n){
			n = f->m_next;	
			delete [] (uint8*)f;	
		}	
		m_head = NULL;
		m_tail = NULL;
		m_w_pos = NULL;
		m_size = 0;
	}
};

