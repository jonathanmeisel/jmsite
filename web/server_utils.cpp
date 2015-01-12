/*
 * server_utils.cpp
 *
 *  Created on: Jan 10, 2015
 *      Author: jonathanmeisel
 */

#include "server.h"
#include "server_utils.h"
#include <sys/stat.h>
#include <sys/resource.h>

#define REQLEN 65535
#define LINELEN 256

// Checks if a string ends with suffix suffix
bool has_suffix(std::string const & str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool has_prefix(std::string const & str, const std::string & prefix)
{
	return str.size() >= prefix.size() &&
			str.compare(0, prefix.size(), prefix) == 0;
}

bool validPath(std::string const & path)
{

	std::string invalid = "..";
	std::size_t found = path.find(invalid);
	return (found == std::string::npos);
}

// Checks if the file exists
bool exists(std::string & name, bool dir)
{
	if (!validPath(name))
	{
		return false;
	}

	struct stat buffer;
	int success = stat(name.c_str(), &buffer);
	if (success == 0 && !dir)
	{
		return S_ISREG(buffer.st_mode);
	}
	if (success == 0 && dir)
	{
		return S_ISDIR(buffer.st_mode);
	}
	return false;
}

// Parses a string into an integer, or returns the default val
int safeParse(std::string const & input, int d)
{
	try
	{
		int out = std::stoi(input);
		return out;
	}
	catch (std::invalid_argument& e)
	{
		return d;
	}
}

void readToMap(std::string const & configFile, std::unordered_map<std::string, std::string>& config)
{
	char line [LINELEN];

	// Open the config file
	std::ifstream file;
	file.open(configFile);

	// Read the key value pairs into the map
	char * cvalue;
	while (!file.fail())
	{
		file.getline(line, LINELEN);
		if (line == NULL)
		{
			continue;
		}

		strtok_r(line, "=", &cvalue);
		if (cvalue == NULL)
		{
			continue;
		}

		std::string key{line};
		std::string value{cvalue};

		config.insert(std::make_pair(key, value));
	}
}

void readConfig(std::string configFile, HttpWebServer& server)
{
	// write configuration into a map
	std::unordered_map<std::string, std::string> config;
	readToMap(configFile, config);

	// Set the appropriate things
	if (config.count("thread_pool_size") > 0)
	{
		int thread_size = safeParse(config.at("thread_pool_size"), -1);
		if (thread_size > 0)
		{
			server.setNumThreads(thread_size);
		}
	}
	if (config.count("queued") > 0)
	{
		int queued = safeParse(config.at("queued"), -1);
		if (queued > 0)
		{
			server.setMaxQueued(queued);
		}
	}
	if (config.count("basedir") > 0)
	{
		std::string dir = config.at("basedir");
		if (exists(dir, true))
		{
			server.setStaticFiles(dir);
		}
	}
	if (config.count("port") > 0)
	{
		std::string sport = config.at("port");
		int port = safeParse(config.at("port"), -1);
		if (port > 0 && port < 65535)
		{
			server.setPort(sport);
		}
	}
	if (config.count("cgi_gateway") > 0)
	{
		std::string gateway = config.at("cgi_gateway");
		server.setGateway(gateway);
	}
	if (config.count("cgi_bin") > 0)
	{
		std::string bin = config.at("cgi_bin");
		if (bin.compare(".") == 0)
		{
			server.setBin("");
		}
		else
		{
			server.setBin(bin);
		}
	}
	if (config.count("cgi_dir") > 0)
	{
		std::string dir = config.at("cgi_dir");
		server.setBinDir(dir);
	}
}
