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
#include "bug.h"
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

void TeamBalancer::chat(const str& text) const
{
	rcon->call("chat " + prefix + text + suffix);
}

void TeamBalancer::tell(siz /*num*/, const str& text) const
{
	// TELL not working - is there a way to do this?
	chat(text);
//	rcon.call("tell " + std::to_string(num) + " " + prefix + text + suffix);
}

bool TeamBalancer::get_snapshot()
{
	//bug_func();
	// GET guid's

	game old_g = g; // previous state

	g.clear();

	str response;
	if(!rcon->call("!listplayers", response))
		return false;

	//bug_var(response);

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

//			if(guid.empty() && ignore_bots)
//				continue;

		g.players[num].num = num;
		g.players[num].guid = guid;

		// Did we just join the game?
		bool joined_game = !old_g.teams[team_id::R].count(num) && !old_g.teams[team_id::B].count(num);

		if(team == "R" && (!testing || !guid.empty())) // only count blue team bots for testing
		{
			if(g.teams[team_id::R].insert(num).second  && (old_g.teams[team_id::B].count(num) || joined_game))
				g.players[num].joined = hr_clk::now();
		}
		else if(team == "B")
		{
			if(g.teams[team_id::B].insert(num).second && (old_g.teams[team_id::R].count(num) || joined_game))
				g.players[num].joined = hr_clk::now();
		}
		else if(team == "S")
		{
			g.teams[team_id::S].insert(num);
		}
	}

	// GET teams/scores/names - neet to match to guids/names

	if(!rcon->call("status", response))
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
			g.mapname = line.substr(5);

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

//			if(ip == "bot" && ignore_bots)
//				continue;

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
//	bug_func();

	if(!policy.get())
		policy = TeamPolicy::create(TeamPolicy::get_null_policy_name()); // default

	if(!policy.get())
	{
		log("ERROR: no policy");
		return;
	}

	str val;
	if(!rconset("rconteam_policy", val))
	{
		log("WARN: No policy set in server");
		val.clear();
	}

	const str_set policies = TeamPolicy::get_policy_names();

	if(policies.find(val) == policies.cend())
	{
		log("WARN: Unknown policy: " << val);
		val = TeamPolicy::get_default_policy_name();
	}

	if(val != policy->name())
	{
		chat("Chanding policy to: ^7" + val);
		policy = TeamPolicy::create(val);
	}

	bool old = enforcing;
	if(!rconset("rconteam_enforcing", enforcing))
		enforcing = old;
	if(enforcing != old)
		chat("Changing policy to " + str(enforcing?"^1ENFORCING":"^2NON-ENFORCING"));

	old = testing;
	if(!rconset("rconteam_testing", testing))
		testing = old;
	if(testing != old)
		chat("Changing policy to " + str(testing?"^2TESTING":"^1LIVE"));

	static const siz_set teamgames = {1, 3, 4, 5, 8, 9, 11, 12};

	old = team_game;
	siz gametype;
	if(!rconset("g_gametype", gametype))
	{
		log("WARN: Failed to get g_gametype:");
		team_game = old;
		return;
	}

	team_game = teamgames.find(gametype) != teamgames.end();
	if(team_game != old)
		chat("System is active for this gametype.");
}

str key(siz num, const team_id& team)
{
	return to_str(team) + "-" + std::to_string(num);
}

void TeamBalancer::run()
{
	bug_func();
	chat("RCONTEAM System online: v0.1-beta");
	if(enforcing)
		chat("RCONTEAM System is ^1ENFORCING");
	if(testing)
		chat("RCONTEAM System is ^2TESTING");

	while(!done)
	{
		std::this_thread::sleep_until(pause);
		pause += delay;

		select_policy();

		if(!team_game)
			continue;

		if(verbose)
			chat("Analizing Teams");

		get_snapshot(); // updage g
		//g.dump(std::cout); // for testing

		if(!policy.get())
		{
			log("ERROR: no policy");
			continue;
		}

		// implement team chages using rcon
		siz num;
		team_id team;
		if(!policy->balance(g, num, team))
		{
			actions.clear();
			continue;
		}

		if(actions[key(num, team)] == ACT_CALL_TEAMS)
			call_teams(num, team);
		else if(actions[key(num, team)] == ACT_REQUEST_PLAYER)
			request_player(num, team);
		else if(actions[key(num, team)] == ACT_PUTTEAM)
			putteam(num, team);
		else
		{
			log("ERROR: Unknown ACTION:   Should never happen.");
			// take reasonable action
			actions[key(num, team)] = ACT_PUTTEAM;
			putteam(num, team);
		}
	}
}

void TeamBalancer::call_teams(siz num, const team_id& team)
{
	chat("Please balance the teams");
	log("call_teams    : " << num << " " << g.players[num].name);
	++actions[key(num, team)]; // escalate
}

void TeamBalancer::request_player(siz num, const team_id& team)
{
	tell(num, "Please balance the teams: " + g.players[num].name);
	log("request_player: " << num << " " << g.players[num].name);
	++actions[key(num, team)]; // escalate
}

void TeamBalancer::putteam(siz num, const team_id& team)
{
	if(!enforcing)
	{
		tell(num, "Please balance the teams: " + g.players[num].name);
		return;
	}
	rcon->call("!putteam " + std::to_string(num) + " " + to_str(team));
	tell(num, "^3SORRY " + g.players[num].name + " ^3but the teams NEEDED balancing");
	tell(num, g.players[num].name + " ^7:^3 This was an ^7AUTOMATED^3 action");
	actions[key(num, team)] = 0;
	log("putteam       : " << num << " " << g.players[num].name);
}

const str TeamBalancer::prefix = "^1-[^3TEAMS^1]-=<(^3";
const str TeamBalancer::suffix = "^1)>=-";

} // oa
