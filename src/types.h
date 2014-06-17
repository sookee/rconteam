//#pragma once
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
#include <chrono>

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

typedef std::istream sis;
typedef std::ostream sos;

typedef std::stringstream sss;
typedef std::istringstream siss;
typedef std::ostringstream soss;

typedef std::fstream sfs;
typedef std::ifstream sifs;
typedef std::ofstream sofs;

// std::getline is used so frequently in parsing that
// I like to have something smaller and quicker :)
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

class slot
{
private:
	siz num;

public:

	static slot bad;
	static slot null;

	slot(): num(bad.num) {}
	explicit slot(siz num): num(num) {}

	bool operator<(const slot& s) const { return num < s.num; }
	bool operator>(const slot& s) const { return num > s.num; }
	bool operator==(const slot& s) const { return num == s.num; }
	bool operator!=(const slot& s) const { return num != s.num; }

	friend sos& operator<<(sos& o, const slot& s) { return o << s.num; }
	friend sis& operator>>(sis& i, slot& s) { return i >> s.num; }

	explicit operator str() const { return std::to_string(num); }
	explicit operator siz() const { return num; }
};

typedef std::set<slot> slot_set;

enum class team {U, R, B, S, bad = -1};

inline
team to_team(const str& s)
{
	return s=="U"?team::U:(s=="R"?team::R:(s=="B"?team::B:(s=="S"?team::S:team::bad)));
}

inline
sis& operator>>(sis& i, team& t)
{
	int s;
	if(i >> s)
	{
		if(s < 0 || s > 3)
			i.setstate(std::ios::failbit);
		t = static_cast<team>(s);
	}
	return i;
}

inline
str to_str(const team& t)
{
	return t == team::R ? "R" : t == team::B ? "B" : "S";
}

struct player
{
//	slot num; // slot
	siz score;
	siz ping;
	str name;
	str guid;
//	team_id team;
	hr_time_point joined;

	player(): /*num(0), */score(0), ping(0), /*team(team_id::U), */joined(hr_clk::now()) {}
};

//typedef siz_set team; // contains guids
//typedef std::map<team_id, team> team_map;

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
	str mapname;
	std::map<team, slot_set> teams;
	std::map<slot, player> players; // num -> player

	game():teams({{team::S,{}}, {team::R,{}}, {team::B,{}}}) {}

	void clear()
	{
		mapname.clear();
		teams[team::S].clear();
		teams[team::R].clear();
		teams[team::B].clear();
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
		for(const slot& num: teams[team::R])
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
				<< " has " << players[num].score << " points"
				<< " and joined: " << oss.str() << " ago."
				<< '\n';
		}
		os << "blue:\n";
		for(const slot& num: teams[team::B])
		{
			oss.str("");
			print_duration(hr_clk::now() - players[num].joined, oss);
			os << '\t' << players[num].guid << ' ' << players[num].name
			<< " has " << players[num].score << " points"
			<< " and joined: " << oss.str() << " ago."
			<< '\n';
		}
		os << "spec:\n";
		for(const slot& num: teams[team::S])
			os << '\t' << players[num].guid << ' ' << players[num].name
				<< " is speccing." << '\n';
	}
};

/*struct to pass information about sql db aroung*/
struct dbinfo
{
	str host;
	str port;
	str user;
	str pass;
	str base;
};

} // oa

#endif /* TEAMPOLICY_H__ */
