/*
 * ProcessManager.cpp
 *
 *  Created on: Nov 9, 2014
 *      Author: jmeisel
 */

#include "ProcessManager.h"
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

// Initialize to no base directory
ProcessManager::ProcessManager(std::string dir)
: m_runningProcesses{}, m_maxProcesses{10}, m_dir{dir}
{ }

ProcessManager::~ProcessManager()
{
	//Kill all the processes
	for (auto it = m_runningProcesses.begin(); it != m_runningProcesses.end(); ++it)
	{
	    if (kill(it->first, false) == -1)
	    {
	    	kill(it->first, true);
	    }
	}
}

void ProcessManager::setMaxProcesses(int maxProcesses)
{
	m_maxProcesses = maxProcesses;
}

// Start the new process (need to reap if synchronous is false)
ProcessData const & ProcessManager::startNewCommand(std::string command, std::unordered_map<std::string, std::string> env)
{
	ProcessData data{};

 	if (command.empty())
	{
		throw std::invalid_argument("The command is empty.");
	}

	// First, split the command into components
	std::vector<std::string> splitCommand;
	boost::split(splitCommand, command, boost::is_any_of(" "));

	if (splitCommand.size() < 1)
	{
		throw std::invalid_argument("Huh?");
	}

	// The executable and its arguments
	//char const * executable;
	char ** args = new char * [splitCommand.size() + 1];

	int i = 0;
	for (std::string& scommand : splitCommand)
	{
		args[i] = (char * ) scommand.c_str();
		i++;
	}

	// Make sure last argument pointer is null
	args[splitCommand.size()] = NULL;

	// Make sure more than max processes aren't running
	while ((m_maxProcesses > 0) && (m_runningProcesses.size() >= m_maxProcesses))
	{
		waitForOne();
	}

	// Pipe
	int pfds[2];
	if (pipe(pfds) == -1)
	{
		throw ProcessError{"Cannot Pipe In"};
	}

	pid_t childPid = fork();

	// the child
	if (childPid == 0)
	{
		chdir(m_dir.c_str());
		
		//set envs
		for ( auto it = env.begin(); it != env.end(); ++it )
		{
			setenv(it->first.c_str(), it->second.c_str(), true);
		}

		// close the read part and open the write part
		// Do errors later
		close(pfds[0]);
		dup2(pfds[1], 1);
		close(pfds[1]);
		execvp((const char *) args[0], (char * const * )args);
		exit(0);
	}
	else
	{
		// close the write, keep the read
		close(pfds[1]);

		data.pid = childPid;
		data.pipefd = pfds[0];
		data.command = std::string{command};

		m_runningProcesses.insert(std::make_pair(childPid, data));
		return m_runningProcesses.at(childPid);
	}
}

// Reaps zombies
void ProcessManager::pollOpenProcesses()
{
	int status;
	pid_t tpid;

	while((tpid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		if (m_runningProcesses.count(tpid) > 0)
		{
			m_runningProcesses.erase(tpid);
		}
	}
}

// Waits for all running process to terminate
void ProcessManager::waitForAll()
{
	int status;
	pid_t tpid;

	while(m_runningProcesses.size() > 0)
	{
		tpid = wait(&status);

		if (m_runningProcesses.count(tpid) > 0)
		{
			m_runningProcesses.erase(tpid);
		}
	}
}

// Wait for a specific process to exit
void ProcessManager::waitFor(ProcessData const & pd)
{
	int status;
	if (m_runningProcesses.count(pd.pid) > 0)
	{
		waitpid(pd.pid, &status, 0);
		m_runningProcesses.erase(pd.pid);
	}
}

void ProcessManager::waitForOne()
{
	int status;
	if (m_runningProcesses.size() > 0)
	{
		pid_t pid = waitpid(-1, &status, 0);
		m_runningProcesses.erase(pid);
	}
}

// Kills a running process
void ProcessManager::killProcess(ProcessData const & pd, bool force)
{
	if (m_runningProcesses.count(pd.pid) > 0)
	{
		close(pd.pipefd);

		if (!force)
		{
			kill(pd.pid, SIGTERM);
		}
		else
		{
			kill(pd.pid, SIGKILL);
		}

		waitFor(pd);
	}
}
