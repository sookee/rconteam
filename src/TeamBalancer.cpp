/*
 * TeamBalancer.cpp
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

#include <thread>

#include "TeamBalancer.h"
#include "types.h"
#include "log.h"
#include "str.h"

namespace oa {

siz get_last_field(const str& line, str& val, char delim = ' ')
{
	val.clear();
	siz pos = line.size() - 1;
	while(pos != str::npos && line[pos] != delim)
		val.insert(0, 1, line[pos--]);
	return pos;
}

void TeamBalancer::chat(const str& text)
{
	rcon.call("chat " + prefix + text + suffix);
}

bool TeamBalancer::get_snapshot()
{
	// GET guid's

	game old_g = g; // previous state

	g.clear();

	str response;
	if(!rcon.call("!listplayers", response))
		return false;

	// !listplayers: 4 players connected:
	//  0 B 0   Unknown Player (*)   Ayumi
	//  1 R 0   Unknown Player (*)   Skelebot
	//  2 R 0   Unknown Player (*)   Major
	//  4 B 0   Unknown Player (*)   Skelebot

	str line;
	siss iss(response);
	sgl(iss, line);
	while(sgl(iss, line))
	{
		str team, guid, skip;
		siz num;

		if(!sgl(sgl(siss(line) >> num >> team, skip, '*'), guid, ')'))
		{
			log("ERROR: parsing !listplayers: " << line);
			return false;
		}

			if(guid.empty() && ignore_bots)
				continue;

		g.players[num].num = num;
		g.players[num].guid = guid;

		if(team == "R" && (!testing || !guid.empty())) // only count blue team bots for testing
		{
			g.R.insert(num);
			// Did we just join the game?
			if(!old_g.R.count(num) && !old_g.B.count(num))
				g.players[num].joined = std::time(0);
		}
		else if(team == "B")
		{
			g.B.insert(num);
			// Did we just join the game?
			if(!old_g.R.count(num) && !old_g.B.count(num))
				g.players[num].joined = std::time(0);
		}
		else if(team == "S")
		{
			g.S.insert(num);
		}
	}

	// GET teams/scores/names - neet to match to guids/names

	if(!rcon.call("status", response))
		return false;

	// map: oasago2
	// num score ping name            lastmsg address               qport rate
	// --- ----- ---- --------------- ------- --------------------- ----- -----
	//   0    27    0 Ayumi^7              50 bot                       0 16384
	//   1    37    0 Skelebot^7           50 bot                       0 16384
	//   2    56    0 Major^7             100 bot                    4769 16384
	//   4    30    0 Skelebot^7          100 bot                      96 16384

	iss.clear();
	iss.str(response);

	while(sgl(iss, line) && !trim(line).empty() && line[0] != '-')
		if(!line.find("map:") && line.size() > 5)
			g.map = line.substr(5);

	while(sgl(iss, line) && !trim(line).empty())
	{
		siz num, score, ping;
		str skip, ip;

		siz pos = get_last_field(line, skip);
		while(pos != str::npos && line[pos] == ' ') --pos;
		if(pos != str::npos)
			pos = get_last_field(line.substr(0, pos + 1), skip);
		while(pos != str::npos && line[pos] == ' ') --pos;
		if(pos != str::npos)
			pos = get_last_field(line.substr(0, pos + 1), ip);
		while(pos != str::npos && line[pos] == ' ') --pos;
		if(pos != str::npos)
			pos = get_last_field(line.substr(0, pos + 1), skip);
		while(pos != str::npos && line[pos] == ' ') --pos;
		std::istringstream iss(line.substr(0, pos + 1));

			if(ip == "bot" && ignore_bots)
				continue;

		if(!(iss >> num >> score >> ping).ignore())
			continue;

		str name; // can be empty, if so keep name from !listplayers
		sgl(iss, name);

		g.players[num].score = score;
		g.players[num].name = name;
	}
	return true;
}

/**
 * Query the server over rcon for the value of various cvars
 * and select the appropriate TeamPolicy inplementation and
 * set various policy control values.
 */
void TeamBalancer::select_policy()
{
	str val;

	rconset("rconteam_policy", val);
	if(!policy.get() || val != policy->name())
	{
		chat("Chanding policy to: ^1" + val);
		policy = TeamPolicy::create(val);
	}

	bool old = enforcing;
	rconset("rconteam_enforcing", enforcing);
	if(enforcing != old)
		chat("Changing policy to " + str(enforcing?"^1ENFORCING":"^2NON-ENFORCING"));

	old = testing;
	rconset("rconteam_testing", testing);
	if(testing != old)
		chat("Changing policy to " + str(testing?"^2TESTING":"^1LIVE"));
}

void TeamBalancer::run()
{
	chat("RCONTEAM System online: v0.1-beta");
	if(enforcing)
		chat("RCONTEAM System is ^1ENFORCING");
	if(testing)
		chat("RCONTEAM System is ^2TESTING");

	while(!done)
	{
		if(verbose)
			chat("Analizing Teams");
		// update game snapshot from rcon
		get_snapshot();
		g.dump(std::cout); // for now

		// select team policy (rcon variable)
		select_policy();

		// implement team chages using rcon
		siz num;
		char team;
		if(policy.get() && policy->action(g, num, team))
		{
			if(actions[std::to_string(num) + team] == ACT_CALL_TEAMS)
				call_teams(num, team);
			else if(actions[std::to_string(num) + team] == ACT_REQUEST_PLAYER)
				request_player(num, team);
			else if(actions[std::to_string(num) + team] == ACT_PUTTEAM)
				putteam(num, team);
		}

		// sleep for a bit
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

void TeamBalancer::call_teams(siz num, char team)
{
	chat("PLEASE BALANCE THE TEAMS");
	log("call_teams    : " << num << " " << g.players[num].name);
	++actions[std::to_string(num) + team]; // escalate
}

void TeamBalancer::request_player(siz num, char team)
{
	chat(g.players[num].name + " ^7PLEASE CHANGE TEAMS!");
	log("request_player: " << num << " " << g.players[num].name);
	++actions[std::to_string(num) + team]; // escalate
}

void TeamBalancer::putteam(siz num, char team)
{
	chat("^7SORRY " + g.players[num].name + " ^7BUT THE TEAMS NEED BALANCING");
	if(enforcing)
	{
		rcon.call("!putteam " + std::to_string(num) + " " + team);
		chat(g.players[num].name + " : THIS WAS AN AUTOMATED ACTION");
	}
	log("putteam       : " << num << " " << g.players[num].name);
	actions.clear(); // reset all players
}

//const str TeamBalancer::prefix = "^3^7TEAM^3^7";
//const str TeamBalancer::suffix = "^3";
const str TeamBalancer::prefix = "^3=[^7TEAM^3]==(^7";
const str TeamBalancer::suffix = "^3)";

} // oa
