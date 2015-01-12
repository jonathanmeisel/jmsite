/*
 * ProcessManager.h
 *
 *  Created on: Nov 9, 2014
 *      Author: jmeisel
 */

#ifndef CLINK2DTN_PROCESSMANAGER_H_
#define CLINK2DTN_PROCESSMANAGER_H_

#include <string>
#include <unordered_map>
#include <unistd.h>
#include <vector>

struct ProcessData
{
	std::string command;
	pid_t pid;
	int pipefd;
};

class ProcessManager
{
public:
	ProcessManager(std::string dir);
	~ProcessManager();

	void pollOpenProcesses(); //Reap all processes that have died
	void waitFor(ProcessData const & pd); // Wait for the specific process
	void waitForOne(); // Wait for the next process to finish
	void waitForAll(); // Wait for Processes);

	void setMaxProcesses(int maxProcesses);

	ProcessData const & startNewCommand(std::string command, std::unordered_map<std::string, std::string> env);
	void killProcess(ProcessData const & pd, bool force); // kill

private:
	std::string m_dir;
	std::unordered_map<pid_t, ProcessData> m_runningProcesses;
	int m_maxProcesses;
};

struct ProcessError
{
	std::string m_message;

	ProcessError(std::string const & message)
	: m_message{message}
	{}
};

#endif /* CLINK2DTN_PROCESSMANAGER_H_ */
