/*
 * server.h
 *
 *  Created on: Jan 1, 2015
 *      Author: jmeisel
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "socket_link2.h"
#include "socket_utils2.h"
#include "blockingqueue.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <vector>
#include <fstream>
#include <memory>

#define DEFAULT_NUM_THREADS 300
#define DEFAULT_MAX_QUEUED 1000
#define DEFAULT_PORT "3000"


class Server;
void acceptorThread(std::shared_ptr<Queue<SockWrapper>> qp, Server * s);

struct ServerError
{
	std::string m_message;

	ServerError(std::string message)
	: m_message{message}
	{}
};

class Server
{
protected:
	std::string log = "server.log";
	std::string m_port;
	int num_threads = DEFAULT_NUM_THREADS;
	int max_queued = DEFAULT_MAX_QUEUED;

public:
	std::shared_ptr<Queue<std::string>> m_messages{new Queue<std::string>{max_queued}};

	Server();
	Server(Server const & copy) = delete;
	Server(Server&& server);
	virtual ~Server();

	// Overriden by class using it
	virtual void handleIncoming(SockWrapper&& wrapper) = 0;

	// Listen for connections, call handleIncoming when they arrive
	void listen();
};



#endif /* SERVER_H_ */
