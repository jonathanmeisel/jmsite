/*
 * DateTime.h
 *
 *  Created on: Jan 15, 2015
 *      Author: jonathanmeisel
 */

#ifndef SERVER_DATETIME_H_
#define SERVER_DATETIME_H_

#include <ctime>
#include <string>

class DateTime
{
	std::time_t m_time;
public:
	// Construct datetime with current date and date
	DateTime();
	std::string toString();
};



#endif /* SERVER_DATETIME_H_ */
