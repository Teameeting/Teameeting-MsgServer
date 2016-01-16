#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/epoll.h>

#include "ev.h"
#include "OS.h"
#include "OSHeaders.h"
#include "MyAssert.h"

/*	<EPOLL 使用注意事项>
 *
 *	epollwait和epollctl都是线程安全的，但是当一个线程epollwait，而另一个线程用epollctl向同一个
 *	epoll_fd添加了一个监测fd后，epollwait有可能被改fd的读/写事件唤醒。
 *	对于epollwait线程来说，可能得到的fd不是期待集合中的内容。这里需要注意一下。
 *	参考:
 *	http://stackoverflow.com/questions/5541054/how-to-correctly-read-data-when-using-epoll-wait
 *	http://stackoverflow.com/questions/7058737/is-epoll-thread-safe
*/

#define MAX_EVENTS 128
static struct epoll_event sEvents[MAX_EVENTS];
static int sEpfd = -1;
static int sNumFDsBackFromEpoll = 0;
static UInt32 sNumFDsProcessed = 0;
//* For bug: 客户端连接时发送消息，服务器不处理
static OSMutex sMaxFDPosMutex;

void select_startevents()
{
	sEpfd = epoll_create(MAX_EVENTS);
	sNumFDsBackFromEpoll = 0;
	sNumFDsProcessed = 0;
}

void select_stopevents()
{
	if(-1 != sEpfd)
	{
		::close(sEpfd);
		sEpfd = -1;
	}
}

int select_removeevent(int fd)
{
	struct epoll_event epEv;
	memset(&epEv, 0, sizeof(epEv));
	epEv.data.fd = fd;
	epEv.data.ptr = NULL;

	{
		OSMutexLocker locker(&sMaxFDPosMutex);
		epoll_ctl(sEpfd,EPOLL_CTL_DEL,fd,&epEv);
	}
	::close(fd);
	return 0;
}

int select_watchevent(struct eventreq *req, int which)
{
	//Add or remove this fd from the specified sets
	struct epoll_event epEv;
	memset(&epEv, 0, sizeof(epEv));

	epEv.events = 0;
    if (which & EV_RE)
		epEv.events |= EPOLLIN;

	if (which & EV_WR)
		epEv.events |= EPOLLOUT;

	// 去除ET模式
	//epEv.events |= EPOLLET;
	epEv.data.fd = req->er_handle;
	epEv.data.ptr = req;

	OSMutexLocker locker(&sMaxFDPosMutex);
	if (epoll_ctl(sEpfd, EPOLL_CTL_ADD, req->er_handle, &epEv) == -1) {
		return -1;
	}

	return 0;
}

int select_modwatch(struct eventreq *req, int which)
{
	//Modify this fd from the specified sets
	struct epoll_event epEv;
	epEv.events = 0;
    if (which & EV_RE)
		epEv.events |= EPOLLIN;

	if (which & EV_WR)
		epEv.events |= EPOLLOUT;

	// 去除ET模式
	//epEv.events |= EPOLLET;
	epEv.data.fd = req->er_handle;
	epEv.data.ptr = req;

	OSMutexLocker locker(&sMaxFDPosMutex);
	if (epoll_ctl(sEpfd, EPOLL_CTL_MOD, req->er_handle, &epEv) == -1) {
		return -1;
	}
	
	return 0;
}

int select_waitevent(struct eventreq *req, void* onlyForMOSX)
{
	//Check to see if we still have some select descriptors to process
    int theFDsProcessed = (int)sNumFDsProcessed;
    
    if (theFDsProcessed < sNumFDsBackFromEpoll)
	{
		struct eventreq* evReq = (struct eventreq*)sEvents[theFDsProcessed].data.ptr;
		req->er_handle = evReq->er_handle;
		req->er_data = evReq->er_data;
		req->er_eventbits = 0;
		if (sEvents[theFDsProcessed].events & EPOLLIN) 
		{
			req->er_eventbits |= EV_RE;
		}
		if (sEvents[theFDsProcessed].events & EPOLLOUT)
		{
			req->er_eventbits |= EV_WR;
		}
		sNumFDsProcessed ++;
		return 0;
	}

	if(sNumFDsProcessed > 0)
	{
		sNumFDsProcessed = 0;
		sNumFDsBackFromEpoll = 0;
	}

	{
		// Listen the sokcets.
		OSMutexLocker locker(&sMaxFDPosMutex);
		sNumFDsBackFromEpoll = epoll_wait(sEpfd, sEvents, MAX_EVENTS, 1);
	}

	if (sNumFDsBackFromEpoll >= 0)
        return EINTR;   //either we've timed out or gotten some events. Either way, force caller
                        //to call waitevent again.
    return sNumFDsBackFromEpoll;
}
