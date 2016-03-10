#include "ef_sock.h"
#include <ctype.h>
#include <errno.h>
#include <cassert>
#include <string.h>
#include <sstream>
#include <net/if.h>
#include "base/ef_utility.h"

namespace ef{

#if defined(GPUSH_LINUX)
#define LINUX_EPOLL
#else
typedef fd_set fdSet;
#endif

std::string ipStr(in_addr_t addr){
	std::stringstream os;
	uint8* p = (uint8*)&addr;
	int fr1 = p[0];
	int fr2 = p[1];
	int fr3 = p[2];
	int fr4 = p[3];
	
	os << fr1 << "." << fr2 << "." << fr3 << "."
		<< fr4;

	return os.str();
}

bool isLocalIP(const std::string& ip)
{
	std::vector<std::string> ret;
	split(ip, ret, ".");

	if(ret[0] == "10" || ret[0] == "172" || ret[0] == "192")
		return true;

	return false;
}


bool isLocalIP(in_addr_t addr)
{
	uint8* p = (uint8*)&addr;
	int fr1 = p[0];
	int fr2 = p[1];
	//int fr3 = p[2];
	//int fr4 = p[3];
	
	switch(fr1){
	case 10:
		return true;
	case 172:
		return (fr2 >= 16 && fr2 <= 31);
	case 192:
		return fr2 == 168;
	}

	return false;
}

int getIPs ( in_addr_t addrs[], int asize )
{
	int MAXINTERFACES=16;
	int i = 0;
	int fd, intrface = 0;
	struct ifreq buf[MAXINTERFACES]; ///if.h
	struct ifconf ifc; ///if.h
	in_addr_t loopback;
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0) //socket.h
		return -1;
	ifc.ifc_len = sizeof buf;
	ifc.ifc_buf = (caddr_t) buf;
	if (ioctl (fd, SIOCGIFCONF, (char *) &ifc) < 0) //ioctl.h
		return -1;
	intrface = ifc.ifc_len / sizeof (struct ifreq); 
	
	loopback = inet_addr("127.0.0.1");
	for (int j = 0; j < intrface && i < asize; ++j)
	{
		if (ioctl (fd, SIOCGIFADDR, (char *) &buf[j]) < 0)
		{
			continue;
		}

		struct sockaddr addr = buf[j].ifr_addr;
			

		if (loopback == 
			((sockaddr_in*)&addr)->sin_addr.s_addr)
		{
			continue;
		}	

		addrs[i] = ((sockaddr_in*)&addr)->sin_addr.s_addr;//types
		++i;

	}
	close (fd);
	return i;
}

int getIPs (std::vector<std::string>& ips)
{

	const int MAX_IP_COUNT = 64;
	in_addr_t addrs[MAX_IP_COUNT];

	int ret = getIPs (addrs, MAX_IP_COUNT);

	for(int i = 0; i < ret; ++i){
		ips.push_back(ipStr(addrs[i]));
	}

	return ret;
}

int getLocalIPs ( in_addr_t addrs[], int asize )
{
	const int MAX_IP_COUNT = 64;
	in_addr_t a[MAX_IP_COUNT];

	int ret = getIPs (a, MAX_IP_COUNT);
	int j = 0;

	for(int i = 0; i < ret && j < asize; ++i){
		if (isLocalIP(a[i]))
		{
			addrs[j] = a[i];
			++j;
		}
	}

	return j;
}

int getLocalIPs (std::vector<std::string>& ips)
{
	const int MAX_IP_COUNT = 64;
	in_addr_t addrs[MAX_IP_COUNT];

	int ret = getIPs (addrs, MAX_IP_COUNT);
	int j = 0;

	for(int i = 0; i < ret; ++i){
		if (isLocalIP(addrs[i]))
		{
			ips.push_back(ipStr(addrs[i]));
			++j;
		}
	}

	return j;
}

int getPublicIPs ( in_addr_t addrs[], int asize )
{
	const int MAX_IP_COUNT = 64;
	in_addr_t a[MAX_IP_COUNT];

	int ret = getIPs (a, MAX_IP_COUNT);
	int j = 0;

	for(int i = 0; i < ret && j < asize; ++i){
		if (!isLocalIP(a[i]))
		{
			addrs[j] = a[i];
			++j;
		}
	}

	return j;
}

int getPublicIPs (std::vector<std::string>& ips)
{
	const int MAX_IP_COUNT = 64;
	in_addr_t addrs[MAX_IP_COUNT];

	int ret = getIPs (addrs, MAX_IP_COUNT);
	int j = 0;

	for(int i = 0; i < ret; ++i){
		if (!isLocalIP(addrs[i]))
		{
			ips.push_back(ipStr(addrs[i]));
			++j;
		}
	}

	return j;

}

int tcpConnectWithTimeout ( const char* ip, int port,
			 int timeout_ms )
{
#if defined(GPUSH_LINUX)
	struct sockaddr_in sin;
	int		s, flags, errorValue, n, epofd, nfds;
	struct epoll_event evt, evts[16];
	socklen_t	errorValue_len;
	memset( &sin, 0, sizeof( sin ) );
	sin.sin_family	  = AF_INET;

	sin.sin_addr	= sockGetHostip(ip);
	sin.sin_port	= htons((short)port);
	errorValue	 = 0;
	errorValue_len	 = sizeof( errorValue );
	if ( ( s = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		return( -1 );
	}
	if ( ( flags = fcntl( s, F_GETFL, 0 ) ) < 0 )
	{
		goto tcpConnect_1;
	}

	if ( fcntl( s, F_SETFL, flags | O_NONBLOCK ) < 0 )
	{
		goto tcpConnect_1;
	}
	if ( ( n = connect( s, ( struct sockaddr * )&sin, sizeof( sin ) ) ) < 0 )
	{
	if ( errno != EINPROGRESS )
	{
		goto tcpConnect_1;
	}
	}
	if ( n == 0 )
	{
		goto tcpConnect_0;
	}
	epofd = epoll_create(16);
	if ( epofd < 0 )
	{
		goto tcpConnect_1;
	}

	evt.data.fd = s;
	evt.events = EPOLLIN | EPOLLOUT;
	if ( epoll_ctl( epofd, EPOLL_CTL_ADD, s, &evt ) < 0 ) 
	{
		close(epofd);
		goto tcpConnect_1;
	}

	nfds = epoll_wait(epofd, evts, 16, timeout_ms);
	for (int i = 0; i < nfds; i++) {
		if (evts[i].data.fd != s) continue;
		if (evts[i].events & EPOLLIN || evts[i].events & EPOLLOUT) {
			if ( getsockopt( s, SOL_SOCKET, SO_ERROR, &errorValue,
			&errorValue_len ) < 0 )
			{
				goto tcpConnect_1;
			}
			close(epofd);
			goto tcpConnect_0;
		}else
		{
			goto tcpConnect_1;
		}
	}	

tcpConnect_0:
	if ( fcntl( s, F_SETFL, flags ) < 0 )
	{
		goto tcpConnect_1;
	}
	if ( errorValue != 0 )
	{
		goto tcpConnect_1;
	}
	n = 1;
	setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, &n, sizeof( n ) );
	return( s );
tcpConnect_1:
	close( s );
	return( -1 );
#else
    printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
	return (0);
#endif
}  /* end of tcpConnect */

SOCKET tcpBindConnect(const char * dsthost, int dstport, const char * localip, int localport)
{
	SOCKET fd;
	int ret;
	struct sockaddr_in cli; 
	struct sockaddr_in lcl; 
	int err;

	int reuseaddr = 1;
	int keepalive = 1;


	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == EF_INVALID_SOCKET) {
		return EF_INVALID_SOCKET;
	}

	lcl.sin_family = AF_INET;
	lcl.sin_addr = sockGetHostip(localip);
	lcl.sin_port = htons(localport);	

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(keepalive));

	cli.sin_family = AF_INET;
	cli.sin_addr = sockGetHostip(dsthost);
	cli.sin_port = htons(dstport);
	if(bind(fd, (struct sockaddr *)&lcl, sizeof(struct sockaddr)) < 0){
		closesocket(fd);
		return EF_INVALID_SOCKET;
	}

	ret = connect(fd, (struct sockaddr *)&cli, sizeof(struct sockaddr));

	if (ret != 0) {
		err = sock_errno;
		if (err != SOCK_EINPROGRESS && err != SOCK_EWOULDBLOCK) {
			closesocket(fd);
			return EF_INVALID_SOCKET;
		}
	}

	return fd;
}

SOCKET tcpConnect(const char * dsthost, int dstport, const char * localip, int localport)
{
	SOCKET fd;
	int ret;
	struct sockaddr_in cli; 
	int err;

	int reuseaddr = 1;
	int keepalive = 1;


	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == EF_INVALID_SOCKET) {
		return EF_INVALID_SOCKET;
	}

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(keepalive));


	cli.sin_family = AF_INET;
	cli.sin_addr = sockGetHostip(dsthost);
	cli.sin_port = htons(dstport);

	ret = connect(fd, (struct sockaddr *)&cli, sizeof(struct sockaddr));

	if (ret != 0) {
		err = sock_errno;
		if (err != SOCK_EINPROGRESS && err != SOCK_EWOULDBLOCK) {
			closesocket(fd);
			return EF_INVALID_SOCKET;
		}
	}

	return fd;
}

SOCKET tcpNbConnect(const char * dsthost, int dstport, 
	const char * localip, int localport, int * consucc)
{
	SOCKET fd;
	int ret;
	struct sockaddr_in cli; 
	int err;

	int reuseaddr = 1;
	int keepalive = 1;

	if (consucc != NULL) {
		*consucc = 0;
	}

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == EF_INVALID_SOCKET) {
		if (consucc != NULL) {
			*consucc = -1;
		}

		return EF_INVALID_SOCKET;
	}

	setSocketNonblocking(fd);

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(keepalive));


	cli.sin_family = AF_INET;
	cli.sin_addr = sockGetHostip(dsthost);
	cli.sin_port = htons(dstport);

	ret = connect(fd, (struct sockaddr *)&cli, sizeof(struct sockaddr));

	if (ret != 0) {
		err = sock_errno;
		if (err != SOCK_EINPROGRESS && err != SOCK_EWOULDBLOCK) {
			if (consucc != NULL) {
				*consucc = -1;
			}

			closesocket(fd);

			return EF_INVALID_SOCKET;
		}
	}

	return fd;
}

SOCKET tcpNbConnect2(struct in_addr dstip, int dstport, const char * localip, 
	int localport, int *consucc)
{
	SOCKET fd;
	int ret;
	int err;
	struct sockaddr_in cli; 

	int reuseaddr = 1;
	int keepalive = 1;

	assert(dstport >= 0 && dstport <= 65535);

	if (consucc != NULL) {
		*consucc = 0;
	}

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd <= 0) {
		if (consucc != NULL) {
			*consucc = -1;
		}

		return EF_INVALID_SOCKET;
	}

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(keepalive));

	setSocketNonblocking(fd);

	cli.sin_family = AF_INET;
	cli.sin_addr = dstip;
	cli.sin_port = htons(dstport);

	ret = connect(fd, (struct sockaddr *) &cli, sizeof(cli));
	if (ret != 0) {
		err = sock_errno;
		if (err != SOCK_EINPROGRESS && err != SOCK_EWOULDBLOCK) {
			if (consucc != NULL) {
				*consucc = -1;	//Á¬½ÓÊ§°Ü
			}
			closesocket(fd);
			return EF_INVALID_SOCKET;
		}
	}

	return fd;
}

int tcpSend(SOCKET fd, const char *sendbuf, int towrite, long waitms, int *actsndnum)
{
	int ret = 0;
	int sendLen = 0;
	int errcode = 0;
	int bytes = 0;
	int restms = 0;
	int sval = 0;
	int usval = 0;
	int cast_ms = 0;
	
	struct timeval tick0;
	struct timeval tick1;

#ifdef LINUX_EPOLL
	int ep = 0;
	struct epoll_event ee;
	struct epoll_event ev_list[16];
#else
	struct timeval tv;
	struct timeval *ptv = NULL;
	fdSet wFDs;
#endif

	if (actsndnum != NULL) {
		*actsndnum = 0;
	}

	if (fd == EF_INVALID_SOCKET) {
		return -1;
	}

	if (sendbuf == NULL || towrite <= 0) {
		return -1;
	}

	if (waitms > 0) {
		gettimeofday(&tick0, NULL);
		restms = waitms;
	}

	sendLen = towrite;
	do {
		bytes = send(fd, sendbuf, sendLen, 0);
		if (bytes > 0) {
			sendLen -= bytes;
			sendbuf += bytes;
			if (actsndnum != NULL) {
				*actsndnum += bytes;
			}

		}
		else if (bytes == -1) {
			
			errcode = sock_errno;

			if (errcode ==  SOCK_EINTR) {

				continue;
			}
			else if (errcode == SOCK_EAGAIN 
				|| errcode == SOCK_EWOULDBLOCK) {
				
				if (waitms > 0) {
					/*check time out time*/
					gettimeofday(&tick1, NULL);
					usval = tick1.tv_usec - tick0.tv_usec;
					sval = tick1.tv_sec - tick0.tv_sec;

					if (usval < 0) {
						sval -= 1;
						usval += 1000000;
					}


					cast_ms = sval * 1000 + usval / 1000;
					restms = waitms - cast_ms;
					
					if (restms <= 0) {
						/*time out*/
						ret = 0;
						break; /*break from do {} while*/
					}

				}
				else if (waitms == 0) {
					/*restms must be -1 when waitms <= 0*/
					restms = -1;
				}
				else {
					restms = -1;
				}
				

#ifdef LINUX_EPOLL
				if (ep == 0) {
					ep = epoll_create(16);
					if (ep < 0) {
						return -1;
					}
					
					memset(&ee, 0, sizeof(struct epoll_event));
					ee.events = EPOLLOUT;
						ee.data.ptr = NULL;
					if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee) == -1) {
						close(ep);
						return -1;
					}
				}

				ret = epoll_wait(ep, ev_list, (int)128, restms);
#else				
				if (restms > 0) {
					tv.tv_sec = restms / 1000;
					tv.tv_usec = (restms % 1000) * 1000;
					ptv = &tv;
				}
				else if (restms == 0) {
					tv.tv_sec = 0;
					tv.tv_usec = 0;
					ptv = &tv;
				}
				else {
					/*infinite*/
					ptv = NULL;
				}

				FD_ZERO(&wFDs);
				FD_SET(fd, &wFDs);
				ret = select(fd + 1, NULL, &wFDs, NULL, ptv);
#endif /*LINUX_EPOLL*/
			

				if (ret == -1) {
					errcode = sock_errno;
					if (errcode == SOCK_EINTR) {
						continue;
					}
					else {
						ret = -1;
						break; /*break from do {} while*/
					}
				}
				else if (ret == 0) {
					/*time out*/
					ret = 0;
					break; /*break from do {} while*/
				}
				else {
#ifdef LINUX_EPOLL
					if (ev_list[0].events & EPOLLOUT) {
						/*fd can be write*/
						continue;
					}
					else {
						ret = -1;
						break;
					}
#else
					if (FD_ISSET(fd, &wFDs)) {
						/*fd can be write*/
						continue;
					}
					else {
						ret = -1;
						break;
					}
#endif /*LINUX_EPOLL*/					
				}

			}
			else {
				/*Unexpected error*/
				ret = -1;
				break; /*break from do {} while*/
			}
		}
		else {
			/*bytes == 0, socket has closed*/
			ret = -1;
			break;
		}
	}while (sendLen > 0);


#ifdef LINUX_EPOLL
	if (ep != 0) {
		close(ep);
	}
#endif

	if (sendLen != 0) {
		return ret;
	}

	return towrite;
}

int tcpReceive(SOCKET fd, char * rcvbuf, int toread, long waitms, int *actrcvnum)
{
	int ret = 0;
	int readLen = 0;
	int errcode = 0;
	int bytes = 0;
	int restms = 0;
	int sval = 0;
	int usval = 0;
	int cast_ms = 0;

	struct timeval tick0;
	struct timeval tick1;

#ifdef LINUX_EPOLL
	int ep = 0;
	struct epoll_event ee;
	struct epoll_event ev_list[16];
#else
	struct timeval tv;
	struct timeval *ptv = NULL;
	fdSet rFDs;
#endif	
	

	if (actrcvnum != NULL) {
		*actrcvnum = 0;
	}

	if (fd == EF_INVALID_SOCKET) {
		return -1;
	}

	if (rcvbuf == NULL || toread <= 0) {
		return -1;
	}

	if (waitms > 0) {
		gettimeofday(&tick0, NULL);
		restms = waitms;
	}
	setSocketNonblocking(fd);

	readLen = toread;
	do {
		bytes = recv(fd, rcvbuf, readLen, 0);
		if (bytes > 0) {
			readLen -= bytes;
			rcvbuf += bytes;
			if (actrcvnum != NULL) {
				*actrcvnum += bytes;
			}

		}
		else if (bytes == -1) {
			
			errcode = sock_errno;

			if (errcode == SOCK_EAGAIN 
				|| errcode == SOCK_EWOULDBLOCK || errcode ==  SOCK_EINTR) {
				
				if (waitms > 0) {
					/*check time out time*/
					gettimeofday(&tick1, NULL);
					usval = tick1.tv_usec - tick0.tv_usec;
					sval = tick1.tv_sec - tick0.tv_sec;

					if (usval < 0) {
						sval -= 1;
						usval += 1000000;
					}

					cast_ms = sval * 1000 + usval / 1000;
					restms = waitms - cast_ms;
					
					if (restms <= 0) {
						/*time out*/					
						ret = 0;
						break; /*break from do {} while*/
					}
					
				}
				else if (waitms == 0) {
					/*restms must be -1 when waitms <= 0*/
					restms = -1;
				}
				else {
					restms = -1;
				}
		

#ifdef LINUX_EPOLL
				if (ep == 0) {
					ep = epoll_create(16);
					if (ep < 0) {
						return -1;
					}
					
					memset(&ee, 0, sizeof(struct epoll_event));
					ee.events = EPOLLIN;
						ee.data.ptr = NULL;
					if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee) == -1) {
						close(ep);
						return -1;
					}
				}

				ret = epoll_wait(ep, ev_list, (int)128, restms);
#else
				if (restms > 0) {
					tv.tv_sec = restms / 1000;
					tv.tv_usec = (restms % 1000) * 1000;
					ptv = &tv;
				}
				else if (restms == 0) {
					tv.tv_sec = 0;
					tv.tv_usec = 0;
					ptv = &tv;				
				}
				else {
					/*infinite*/
					ptv = NULL;
				}
			
				FD_ZERO(&rFDs);
				FD_SET(fd, &rFDs);
				ret = select(fd + 1, &rFDs, NULL, NULL, ptv);
#endif /*LINUX_EPOLL*/
			

				if (ret == -1) {
					errcode = sock_errno;
					if (errcode == SOCK_EINTR) {
						continue;
					}
					else {
						ret = -1;
						break; /*break from do {} while*/
					}
				}
				else if (ret == 0) {
					/*time out*/
					ret = 0;
					break; /*break from do {} while*/
				}
				else {
#ifdef LINUX_EPOLL					
					if (ev_list[0].events & EPOLLIN) {
						/*fd can be write*/
						continue;
					}
					else {
						ret = -1;
						break;
					}
#else
					if (FD_ISSET(fd, &rFDs)) {
						/*fd can be write*/
						continue;
					}
					else {
						ret = -1;
						break;
					}
#endif
				}

			}
			else {
				/*Unexpected error*/
				ret = -1;
				break; /*break from do {} while*/
			}
		}
		else {
			/*bytes == 0, socket has closed*/
			ret = -1;
			break;
		}
	}while (readLen > 0);


#ifdef LINUX_EPOLL
	if (ep != 0) {
		close(ep);
	}
#endif

	if (readLen != 0) {
		return ret;
	}

	return toread;
}

struct in_addr sockGetHostip (const char *host)
{
	struct hostent *remoteHost;
	struct in_addr addr;

	addr.s_addr = INADDR_NONE;

	if (host == NULL) {
		return addr;
	}

	if (isalpha(host[0])) {
		remoteHost = gethostbyname(host);
		if (remoteHost == NULL) {
			return addr;
		}

		addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
	}
	else {
		addr.s_addr = inet_addr(host);
	}
	
	return addr;
}

int sockGetPending (SOCKET fd)
{

	int ret;

	int nread;
	ret = ioctl(fd, FIONREAD, &nread);

	if (ret != 0) {
		return 0;
	}

	return nread;
}

int sockReadReady(SOCKET fd, int ms)
{
	int ret;

#ifdef LINUX_EPOLL
	int ep = 0;
	uint32 revents;
	struct epoll_event ee;
	struct epoll_event ev_list[16];

	ep = epoll_create(16);
	if (ep < 0) {
		return 0;
	}

	memset(&ee, 0, sizeof(struct epoll_event));
	ee.events = EPOLLIN;
	ee.data.ptr = NULL;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee) == -1) {
		close(ep);
		return 0;
	}

	ret = epoll_wait(ep, ev_list, (int)128, ms);
	if (ret != 1) {
		close(ep);
		return 0;
	}

	revents = ev_list[0].events;
	if (revents & EPOLLIN) {
		close(ep);
		return 1;
	}

	close(ep);

	return 0;
	
#else
	
	fdSet fdset;
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms % 1000 * 1000;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);

	ret = select(fd + 1, &fdset, NULL, NULL, &tv);
	if (ret > 0) {
		if (FD_ISSET(fd, &fdset)) {
			return 1;
		}
	}

#endif

	return 0;
}
	
int sockWriteReady(SOCKET fd, int ms)
{
	int ret;

#ifdef LINUX_EPOLL
	int ep = 0;
	uint32 revents;
	struct epoll_event ee;
	struct epoll_event ev_list[16];

	ep = epoll_create(16);
	if (ep < 0) {
		return 0;
	}

	memset(&ee, 0, sizeof(struct epoll_event));
	ee.events = EPOLLOUT;
	ee.data.ptr = NULL;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee) == -1) {
		close(ep);
		return 0;
	}

	ret = epoll_wait(ep, ev_list, (int)128, ms);
	if (ret != 1) {
		close(ep);
		return 0;
	}

	revents = ev_list[0].events;
	if (revents & EPOLLOUT) {
		close(ep);
		return 1;
	}

	close(ep);

	return 0;
	
#else

	fdSet fdset;
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms % 1000 * 1000;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);

	ret = select(fd + 1, NULL, &fdset, NULL, &tv);
	if (ret > 0) {
		if (FD_ISSET(fd, &fdset)) {
			return 1;
		}
	}
#endif	

	return 0;
}

int tcpNbReceive (SOCKET fd, char *rcvbuf, int bufsize, int *actrcvnum)
{
	int ret = 0;
	int readLen = 0;
	int errcode = 0;

	if (actrcvnum != NULL) {
		*actrcvnum = 0;
	}

	if (fd == EF_INVALID_SOCKET 
		|| rcvbuf == NULL 
		|| bufsize <= 0) {
		return -1;
	}

	setSocketNonblocking(fd);
	readLen = 0;

	do {
		ret = recv(fd, rcvbuf, bufsize, 0);
		if (ret > 0) {
			readLen += ret;
			if (actrcvnum != NULL) {
				*actrcvnum += ret;
			}

			rcvbuf += ret;
			bufsize -= ret;
		}
		else if (ret == 0) {
			/*Socket closed*/
			return -1;
		}
		else if (ret == SOCKET_ERROR) {
			
			errcode = sock_errno;
			
			if (errcode == SOCK_EINTR) {
				continue;
			}
			else if (errcode == SOCK_EAGAIN 
				|| errcode == SOCK_EWOULDBLOCK) {
				/*No data can be read*/
				break;
			}
			else {
				/*Unexpected error*/
				return -1;
			}
		}
	}while(bufsize > 0);

	return readLen;
}

int setSocketNonblocking(int fd)
{
	unsigned long nonblocking = 1;
	ioctl(fd, FIONBIO, (void *)&nonblocking);
	return 0;
}


SOCKET tcpServerCreate (const char * localIp, int port)
{

	int fd;
	struct sockaddr_in srv; 

	int reuseaddr = 1;
	int keepalive = 1;

	assert(port >= 0 && port <= 65535);

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd <= 0) {
		return EF_INVALID_SOCKET;
	}

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(keepalive));

	srv.sin_family = AF_INET;
	if (strcmp(localIp, "0.0.0.0") == 0) {
		srv.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		srv.sin_addr.s_addr = inet_addr(localIp);
	}
	srv.sin_port = htons(port);

	if (bind( fd, (struct sockaddr *) &srv, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		closesocket(fd);
		return SOCKET_ERROR;
	}

	if (listen( fd, 1024 ) == SOCKET_ERROR) {
		closesocket(fd);
		return SOCKET_ERROR;
	}

	return fd;
}

SOCKET udpServerCreate (struct in_addr * localip, int port)
{
	SOCKET	fd;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		return EF_INVALID_SOCKET;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	if (localip != NULL) {
		addr.sin_addr = *localip;
	}
	else {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	}
	addr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0) {
		sock_close(fd);
		return EF_INVALID_SOCKET;
	}


	return fd;
}

int sockSetBlocking(SOCKET socket, int blocking)
{
	unsigned long nonblocking;

	if (blocking == 0) {
		nonblocking = 1;
	}
	else {
		nonblocking = 0;
	}
	
	ioctl(socket, FIONBIO, (void *)&nonblocking);

	return 0;
}

#ifdef UNIX
/*
return value:
	0 socket is invalid
	1 socket is valid
*/
int sockIsValid(SOCKET fd)
{

#ifdef LINUX_EPOLL
	int  ret;
	int ep = 0;
	uint32 revents;
	struct epoll_event ee;
	struct epoll_event ev_list[16];

	ep = epoll_create(16);
	if (ep < 0) {
		return 0;
	}

	memset(&ee, 0, sizeof(struct epoll_event));
	ee.events = EPOLLIN;
	ee.data.ptr = NULL;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ee) == -1) {
		close(ep);
		return 0;
	}

	ret = epoll_wait(ep, ev_list, (int)128, 0);

	close(ep);
	
	if (ret == -1) {
		return 0;
	}
	else 	if (ret == 0) {
		return 1;
	}
	else if (ret > 0) {
		revents = ev_list[0].events;
		if (revents & EPOLLIN) {
			return 1;
		}
		else {
			return 0;
		}
	}

	return 0;
	
#else

	fdSet rfds;
	int retval = 0, len = 0;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;	

	retval = select(fd + 1, &rfds, NULL , NULL, &tv);
	if (retval == -1) 
		return 0;
	
	if (retval == 0) 
		return 1;
	
	if (!FD_ISSET(fd, &rfds)) 
		return 1;

	retval= ioctl (fd,FIONREAD,&len);

	if ( retval < 0) 
		return 0;

	if (len<=0)
		return 0;
	else
		return 1;

#endif /*LINUX_EPOLL*/

}



#endif

}

