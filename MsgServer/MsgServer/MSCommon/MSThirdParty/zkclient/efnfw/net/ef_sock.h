#ifndef __EF_SOCK_H__
#define __EF_SOCK_H__

#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#if defined(GPUSH_LINUX)
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "base/ef_btype.h"

namespace ef{

#define SOCK_EINPROGRESS		EINPROGRESS      
#define SOCK_EWOULDBLOCK		EWOULDBLOCK
#define SOCK_EAGAIN			EAGAIN
#define SOCK_EINTR			EINTR
#define SOCK_EBADF			EBADF
#define SOCK_EINVAL			EINVAL
#define SOCK_ENOTSOCK			ENOTSOCK

#define sock_errno			errno

#define sock_close			close

#define closesocket			close

#define SOCKET_ERROR			(-1)

typedef	int	SOCKET;

/* Check if socket is in blocking or non-blocking mode.  Return -1 for
 * error, 0 for nonblocking, 1 for blocking. */
int sockQueryBlocking(int socket);

int sockIsValid (SOCKET fd);

#ifndef EF_INVALID_SOCKET
#define EF_INVALID_SOCKET -1
#endif


/* Set socket to blocking or non-blocking mode.  Return -1 for error,
 * 0 for success. */
int sockSetBlocking(SOCKET socket, int blocking);

int setSocketNonblocking(int fd);

bool isLocalIP(in_addr_t addr);
bool isLocalIP(const std::string& ip);
std::string ipStr(in_addr_t addr);
int getIPs(in_addr_t addrs[], int asize);
int getIPs(std::vector<std::string>& ips);
int getLocalIPs(in_addr_t addrs[], int asize);
int getLocalIPs(std::vector<std::string>& ips);
int getPublicIPs(in_addr_t addrs[], int asize);
int getPublicIPs(std::vector<std::string>& ips);

struct in_addr sockGetHostip (const char * host);

/* get the peer ip address from the socket FD */
struct in_addr sockGetPeerIp(SOCKET s);

/* determine socket is open or not */
//int sockIsOpen (SOCKET fd);

/* get the number of the pending bytes from the socket FD */
int sockGetPending (SOCKET fd);

/* determine if the socket fd is read already, the 2nd para is milliseconds waiting for */
int sockReadReady (SOCKET fd, int ms);
int sockWriteReady (SOCKET fd, int ms);


/* Open a server socket. Return -1 for error, >= 0 socket number for OK.*/
SOCKET tcpServerCreate (const char * localIp, int port);

/* Open a client socket with binding a local address. */
SOCKET tcpConnectWithTimeout (const char * dsthost, int dstport, int timeout_ms);
SOCKET tcpConnect (const char * dsthost, int dstport, const char * localip, int localport);
SOCKET tcpBindConnect (const char * dsthost, int dstport, const char * localip, int localport);

/* As above, but given a destination ip address */
SOCKET tcpConnect2 (struct in_addr dstip, int dstport, const char * localip, int localport);


SOCKET tcpNbConnect (const char * dsthost, int dstport, 
                       const char * localip, int localport, int * consucc);
SOCKET tcpNbConnect2 (struct in_addr dstip, int dstport, 
                        const char * localip, int localport, int * consucc);

SOCKET udpServerCreate (struct in_addr * localip, int port);

int tcpReceive    (SOCKET fd, char * rcvbuf, int toread, long waitms, int * actrcvnum);
int tcpNbReceive (SOCKET fd, char * rcvbuf, int bufsize, int * actrcvnum);
int tcpSend       (SOCKET fd, const char * sendbuf, int towrite, long waitms, int * actsndnum);


}

#endif

