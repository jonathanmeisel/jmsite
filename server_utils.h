/*
 * server_utils.h
 *
 *  Created on: Jan 10, 2015
 *      Author: jonathanmeisel
 */

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

#include "server.h"
#include "http.h"
#include <unordered_map>

class HttpWebServer : public Server
{
public:
	std::vector<char> m_notFound;
	std::string m_gateway;
	std::string m_bin;
	std::string m_bindir;
	std::string m_staticFiles;

	HttpWebServer();
	~HttpWebServer();

	void setNumThreads(int threads);
	void setMaxQueued(int max);
	void setPort(std::string port);
	void setGateway(std::string gateway);
	void setBin(std::string bin);
	void setBinDir(std::string binDir);
	void setStaticFiles(std::string dir);
	void setEnvs(HttpRequest const & request, std::unordered_map<std::string, std::string>& env);
	void sendResponseFunction(SockWrapper&& wrapper, HttpRequest const & request);
	virtual void handleIncoming(SockWrapper&& wrapper);
};

bool has_suffix(std::string const & str, const std::string &suffix);
bool has_prefix(std::string const & str, const std::string & prefix);
bool validPath(std::string const & path);
bool exists(std::string & name, bool dir);
int safeParse(std::string const & input, int d);
void readToMap(std::string const & configFile, std::unordered_map<std::string, std::string>& config);
void readConfig(std::string configFile, HttpWebServer& server);




#endif /* SERVER_UTILS_H_ */
