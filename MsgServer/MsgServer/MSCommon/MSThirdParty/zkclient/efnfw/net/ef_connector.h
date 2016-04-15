#ifndef EF_CONNECTOR_H
#define EF_CONNECTOR_H

#include "ef_common.h"
#include "ef_sock.h"
#include "ef_device.h"

namespace ef{

class EventLoop;
class Connection;
class ConnectionFactory;

class Connector:public Device{
public:
private:
	Acceptor(ConnectionFactory *confac = NULL);
	virtual ~Acceptor();
};

};

#endif/*EF_CONNECTOR_H*/
