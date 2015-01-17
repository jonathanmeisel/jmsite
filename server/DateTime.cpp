/*
 * DateTime.cpp
 *
 *  Created on: Jan 15, 2015
 *      Author: jonathanmeisel
 */

#include "DateTime.h"

DateTime::DateTime()
: m_time{std::time(nullptr)}
{}

std::string DateTime::toString()
{
	std::string str{ std::asctime(std::localtime(&m_time))};
	return str.substr(0, str.size() - 1);
}


