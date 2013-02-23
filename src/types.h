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
#include <array>

namespace oa {

// basic types
typedef std::string str;
typedef std::size_t siz;

// containers
typedef std::set<str> str_set;
typedef std::set<siz> siz_set;

typedef std::vector<str> str_vec;
typedef std::vector<siz> siz_vec;

typedef std::map<str, str> str_map;
typedef std::map<siz, siz> siz_map;
typedef std::map<str, siz> str_siz_map;
typedef std::map<siz, str> siz_str_map;

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

class team_id
{
	friend class std::map<team_id, team>;
	siz idx;
	team_id(siz idx): idx(idx){}

	static const str map;
	static const std::vector<team_id> ids;
	static const team_id X;// = -1;

public:
	static const team_id S;// = 0;
	static const team_id R;// = 1;
	static const team_id B;// = 2;

	team_id(): idx(-1){}
	team_id(const team_id& id): idx(id.idx){}

	const team_id& operator!() const { return *this; }

	operator siz() const { return idx; }
	operator str() const { return str(1, map.at(idx)); }
	operator char() const { return map.at(idx); }

	bool operator<(const team_id& id) const { return idx < id.idx; }
	bool operator>(const team_id& id) const { return idx > id.idx; }
	bool operator<=(const team_id& id) const { return idx <= id.idx; }
	bool operator>=(const team_id& id) const { return idx >= id.idx; }
	bool operator==(const team_id& id) const { return idx == id.idx; }

	team_id& operator=(const team_id& id) { idx = id.idx; return *this; }


	// There must be begin and end methods that operate on that structure,
	// either as members or as stand-alone functions, and that return
	// iterators to the beginning and end of the structure

	// The iterator itself must support an operator* method, an operator != method,
	// and an operator++ method, either as members or as stand-alone functions
	// (you can read more about operator overloading)

	class iterator
	{
		friend iterator begin(const team_id&);
		friend iterator end(const team_id&);
	private:
		siz i = 0;
		iterator(siz i = 3): i(i) {}; // 3 = end

	public:
		const team_id& operator*() const { return ids[i]; }
		bool operator!=(const iterator& iter) const { return i != iter.i; }
		iterator& operator++() { i = i + 1 > 3 ? 0 : i + 1; return *this; }

	};
};

inline team_id::iterator begin(const team_id&) { return team_id::iterator(0); }
inline team_id::iterator end(const team_id&) { return team_id::iterator(); }

typedef std::map<team_id, team> team_map;

struct game
{
	str map;
	std::array<siz_set, 3> teams;

	std::map<siz, player> players; // num -> player
	game() {}

	void clear()
	{
		map.clear();
		teams[team_id::S].clear();
		teams[team_id::R].clear();
		teams[team_id::B].clear();
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
		for(const siz& num: teams[team_id::R])
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
				<< " has " << players[num].score << " points"
				<< " and joined: " << oss.str() << " ago."
				<< '\n';
		}
		os << "blue:\n";
		for(const siz& num: teams[team_id::B])
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
			<< " has " << players[num].score << " points"
			<< " and joined: " << oss.str() << " ago."
			<< '\n';
		}
		os << "spec:\n";
		for(const siz& num: teams[team_id::S])
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
