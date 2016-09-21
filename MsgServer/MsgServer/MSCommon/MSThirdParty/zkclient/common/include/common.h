#ifndef __COMMON_COMMON_H__
#define __COMMON_COMMON_H__

#include <vector>
#include <json/json.h>

namespace gim{

typedef struct _PortConfig{
	int MTYPE;
	union {
		struct {
			int ListenWebcon;
			int ListenModule;
			int ListenClicon;
		}connector;
		struct {
			int AcceptConn;
			int ListenDisp;
			int ListenHttp;
		}dispatcher;
		struct {
			int AcceptConn;
			int AcceptDisp;
			int AcceptHttp;
		}meeting;
		struct {
			int ListenClicon;
		}sequence;
	};
}PortConfig;

}

#endif/*__COMMON_COMMON_H__*/
