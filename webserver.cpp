/*
 * webserver.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: jonathanmeisel
 */

#include "server_utils.h"
#include "ProcessManager.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

static std::unordered_map<std::string, std::string> mimetypes{};


void sendResponse(SockWrapper&& wrapper, HttpResponse& response, std::string& file)
{
	wrapper.sendM(response.unparse());
	sendRawFile(wrapper, file);
}

HttpWebServer::HttpWebServer()
: Server{}
{
	HttpResponse notFound{"text/plain", NOT_FOUND};
	std::string sNotFound = std::string{"Not Found"};
	notFound.m_body = std::vector<char>{sNotFound.begin(), sNotFound.end()};
	m_notFound = notFound.unparse();
};

HttpWebServer::~HttpWebServer()
{}

void HttpWebServer::setNumThreads(int threads)
{
	num_threads = threads;
}

void HttpWebServer::setMaxQueued(int max)
{
	max_queued = max;
}

void HttpWebServer::setPort(std::string port)
{
	m_port = port;
}

void HttpWebServer::setGateway(std::string gateway)
{
	m_gateway = gateway;
}

void HttpWebServer::setBin(std::string bin)
{
	m_bin = bin;
}

void HttpWebServer::setBinDir(std::string binDir)
{
	m_bindir = binDir;
}

void HttpWebServer::setStaticFiles(std::string dir)
{
	m_staticFiles = dir;
}

void HttpWebServer::setEnvs(HttpRequest const & request, std::unordered_map<std::string, std::string>& env)
{
	//Copy path, remove '/site'
	std::string path = std::string{request.m_path};
	boost::replace_first(path, m_bin, "");
	env.insert(make_pair("QUERY_PATH", std::move(path)));

	std::string params = std::string{request.m_params};
	env.insert(make_pair("QUERY_PARAMS", std::move(params)));
}

void HttpWebServer::sendResponseFunction(SockWrapper&& wrapper, HttpRequest const & request)
{
	thread_local ProcessManager pm{m_bindir};

	// Set environmental variables
	std::unordered_map<std::string, std::string> env{};
	setEnvs(request, env);

	ProcessData child = pm.startNewCommand(m_gateway, env);

	std::vector<char> message;
	HttpResponse response{"text/html", OK};
	wrapper.sendM(response.unparse());

	while (true)
	{
		message.resize(1024);
		int recvd = read(child.pipefd, message.data(), 1024);
		if (recvd <= 0)
		{
			break;
		}
		message.resize(recvd);
		wrapper.sendM(message);
	}

	wrapper.closeSock();
	pm.waitFor(child);
}

void HttpWebServer::handleIncoming(SockWrapper&& wrapper)
{
	// Receive the HTTP header
	std::vector<char> message;
	std::vector<char> delim{{'\r', '\n', '\r', '\n'}};
	int size = recvTo(wrapper, message, delim, 2048);

	// Couldn't receive header
	if (size <= 0)
	{
		wrapper.sendM(m_notFound);
		return;
	}
	try
	{
		// get the path, add index if necessary
		HttpRequest requestR = parseRequest(message, false);

		if (requestR.m_type == POST)
		{
			int length = 0;
			std::shared_ptr<HttpHeader> header = requestR.getHeader("Content-Length");
			if (header == nullptr)
			{
				throw HttpException{"No Content Length"};
			}

			try
			{
				length = std::stoi(header->m_value);
			}
			catch (...)
			{
				throw HttpException{"Can't parse content length"};
			}

			wrapper.recvAll(requestR.m_body, length);
		}

		if (requestR.m_path.compare("/") == 0)
		{
			requestR.m_path += "index";
		}
		std::string path = requestR.m_path;

		// Now, get the suffix
		std::size_t index = path.find_last_of('.');
		std::string suffix;
		if (index != std::string::npos)
		{
			suffix = path.substr(index);
		}

		// If we need to call a function
		if (suffix.compare(m_bin) == 0)
		{
			sendResponseFunction(std::move(wrapper), requestR);
			return;
		}

		// Otherwise, assume we're dealing with static files here
		path = m_staticFiles + path;
		if (!exists(path, false))
		{
			wrapper.sendM(m_notFound);
			return;
		}

		if (mimetypes.count(suffix) > 0)
		{
			HttpResponse response{mimetypes.at(suffix), OK};
			sendResponse(std::move(wrapper), response, path);
		}
	}
	catch (HttpException& e)
	{
		std::cout << e.m_cause << std::endl;
		wrapper.sendM(m_notFound);
	}
}

void loadMimeTypes()
{
	readToMap("mime_types", mimetypes);
}

int main(int argc, char ** argv)
{
	try
	{
		HttpWebServer server{};
		readConfig("server.cfg", server);
		loadMimeTypes();
		server.listen();
	}
	catch (SocketError& e)
	{
		std::cout << e.m_message << std::endl;
	}
}
