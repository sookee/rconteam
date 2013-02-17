#pragma once
#ifndef _OA_TYPES_H_
#define _OA_ TYPES_H_

/*
 * types.h
 *
 *  Created on: Feb 17, 2013
 *      Author: oasookee@gmail.com
 */

/*-----------------------------------------------------------------.
| Copyright (C) 2013 SooKee oaskivvy@gmail.com                     |
|               2013 w!ng                                          |
'------------------------------------------------------------------'

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.

http://www.gnu.org/licenses/gpl-2.0.html

'-----------------------------------------------------------------*/

#include <string>
#include <set>
#include <map>
#include <ctime>
#include <mutex>
#include <fstream>
#include <sstream>

namespace oa {

// basic types
typedef std::string str;
typedef std::size_t siz;

// containers
typedef std::set<str> str_set;

// time
typedef std::chrono::steady_clock st_clk;
typedef st_clk::period st_period;
typedef st_clk::time_point st_time_point;

typedef std::chrono::high_resolution_clock hr_clk;
typedef hr_clk::period hr_period;
typedef hr_clk::time_point hr_time_point;

// threads
typedef std::lock_guard<std::mutex> lock_guard;

// streams

typedef std::stringstream sss;
typedef std::istringstream siss;
typedef std::ostringstream soss;

typedef std::fstream sfs;
typedef std::ifstream sifs;
typedef std::ofstream sofs;

inline
std::istream& sgl(std::istream& is, str& s, char d = '\n')
{
	return std::getline(is, s, d);
}

inline
std::istream& sgl(std::istream&& is, str& s, char d = '\n')
{
	return sgl(is, s, d);
}

// project types
struct player
{
	str guid;
	siz score;
	str name;
	std::time_t joined;
};

typedef str_set team; // contains guids

struct game
{
	// guids
	team R;
	team B;
	team S;

	std::map<str, player> players; // guid -> player

	void dump(std::ostream& os)
	{
		os << "red:\n";
		for(const str& guid: R)
			os << '\t' << players[guid].name << " has " << players[guid].score << " points." << '\n';

		os << "blue:\n";
		for(const str& guid: B)
			os << '\t' << players[guid].name << " has " << players[guid].score << " points." << '\n';

		os << "spec:\n";
		for(const str& guid: S)
			os << '\t' << players[guid].name << " has " << players[guid].score << " points." << '\n';
	}
};

/*struct to pass information about sql db aroung*/
struct dbinfo
{
	str host;
	str port;
	str login;
	str password;
	str dnName;
};


} // oa

#endif /* TEAMPOLICY_H_ */
