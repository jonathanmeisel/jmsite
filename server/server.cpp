/*
 * server.h
 *
 *  Created on: Dec 15, 2014
 *      Author: jonathanmeisel
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "server.h"
#include "DateTime.h"

void loggingThread(std::shared_ptr<Queue<std::string>> qp, std::string logfile)
{
	std::ofstream file;
	file.open(logfile, std::ios::app);
	while (true)
	{
		std::string out = qp->pop();

		DateTime time{};
		std::string timestr = time.toString();

		file << timestr << ": " << out << std::endl;
	}
}

/* This class implements an HTTP server with a thread pool.
 * To use it, it should be subclassed and handleIncoming overriden.
 */

Server::Server()
: m_port{DEFAULT_PORT}
{}

Server::~Server()
{}


void Server::listen()
{
	std::thread logger{loggingThread, m_messages, log};

	// Create a server listening on the specific port
	SockServer sserver{m_port, 10};
	std::shared_ptr<Queue<SockWrapper>> qp{new Queue<SockWrapper>{max_queued}};

	// start the right number of responder threads
	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; i++)
	{
		threads.push_back(std::thread{acceptorThread, qp, this});
	}

	std::cout << "Listening for connections on Port: " << m_port  << " with a thread pool of size " << num_threads << std::endl;

	// Continually listen for new connections, and add them to a responder thread
	while (true)
	{
		try
		{
			SockWrapper wrapper = sserver.acceptConnection();
			qp->push(std::move(wrapper));
		}
		catch (SocketError& e)
		{
			m_messages->push(e.m_message);
		}
	}
}

void acceptorThread(std::shared_ptr<Queue<SockWrapper>> qp, Server * s)
{
	while (true)
	{
		SockWrapper wrapper = qp->pop();
		s->handleIncoming(std::move(wrapper));
	}
}

#endif /* SERVER_HPP_ */
