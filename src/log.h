#pragma once
#ifndef _OA_LOG_H_
#define _OA_LOG_H_

/*
 * log.h
 *
 *  Created on: Feb 17, 2013
 *      Author: oasookee@gmail.com
 */

#include <iostream>
#include <ctime>

namespace oa {

/*
 * Basic logging.
 */

inline
std::string get_stamp()
{
	time_t rawtime = std::time(0);
	tm* timeinfo = std::localtime(&rawtime);
	char buffer[32];
	std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", timeinfo);

	return std::string(buffer);
}

inline
std::ostream& botlog(std::ostream* os = 0)
{
	static std::ostream* osp = 0;

	// initialize
	if(!osp)
		if(!os)
			osp = &std::cout;

	// change
	if(os)
		osp = os;

	return *osp;
}

#define log(m) do{oa::botlog() << oa::get_stamp() << ": " << m << std::endl;}while(false)

// Console output
#define con(m) do{std::cout << m << std::endl;}while(false)

} // oa

#endif /* _OA_LOG_H_ */
