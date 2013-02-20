#pragma once
#ifndef _OA_TYPES_H__
#define _OA_TYPES_H__

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
#include <vector>

namespace oa {

// basic types
typedef std::string str;
typedef std::size_t siz;

// containers
typedef std::set<str> str_set;
typedef std::vector<str> str_vec;

typedef std::set<siz> siz_set;
typedef std::vector<siz> siz_vec;

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
	siz num; // slot
	siz score;
	siz ping;
	str name;
	str guid;
	char team;
	hr_time_point joined;

	player(): num(0), score(0), ping(0), team('S'), joined(hr_clk::now()) {}
};

typedef siz_set team; // contains guids

// TODO: mve to utils.h
template<typename Rep, typename Period>
void print_duration(std::chrono::duration<Rep, Period> t, std::ostream& os)
{
	typedef std::chrono::duration<int, std::ratio<60 * 60 * 24>> days;

	auto d = std::chrono::duration_cast < days > (t);
	auto h = std::chrono::duration_cast < std::chrono::hours > (t - d);
	auto m = std::chrono::duration_cast < std::chrono::minutes > (t - d - h);
	auto s = std::chrono::duration_cast < std::chrono::seconds > (t - d - h - m);
	if(t >= days(1))
		os << d.count() << "d ";
	if(t >= std::chrono::hours(1))
		os << h.count() << "h ";
	if(t >= std::chrono::minutes(1))
		os << m.count() << "m ";
	os << s.count() << "s";
}

struct game
{
	str map;

	// nums (slot)
	team R;
	team B;
	team S;

	std::map<siz, player> players; // num -> player

	void clear()
	{
		map.clear();
		R.clear();
		B.clear();
		S.clear();
//		players.clear();
	}

	/**
	 * For testing
	 * @param os Where to send the output
	 */
	void dump(std::ostream& os)
	{
		soss oss;
		os << "red:\n";
		for(const siz& num: R)
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
				<< " has " << players[num].score << " points"
				<< " and joined: " << oss.str() << " ago."
				<< '\n';
		}
		os << "blue:\n";
		for(const siz& num: B)
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
			<< " has " << players[num].score << " points"
			<< " and joined: " << oss.str() << " ago."
			<< '\n';
		}
		os << "spec:\n";
		for(const siz& num: S)
			os << '\t' << players[num].guid << ' ' << players[num].name
				<< " is speccing." << '\n';
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

#endif /* TEAMPOLICY_H__ */
