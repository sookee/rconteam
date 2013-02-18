/*
 * rconteam.cpp
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
#include <chrono>

#include "log.h"
#include "str.h"
#include "rcon.h"
#include "types.h"
#include "TeamPolicy.h"

using namespace oa;

siz get_last_field(const str& line, str& val, char delim = ' ')
{
	val.clear();
	siz pos = line.size() - 1;
	while(pos != str::npos && line[pos] != delim)
		val.insert(0, 1, line[pos--]);
	return pos;
}

class TeamBalancer
{
private:
	bool done = false;

	const RCon rcon;
	TeamPolicySPtr policy;

	game g; // current snapshot

	enum
	{
		ACT_CALL_TEAMS, ACT_REQUEST_PLAYER, ACT_PUTTEAM
	};

	typedef std::map<str, siz> action_map;
	action_map actions;

	void call_teams(siz num, char team);
	void request_player(siz num, char team);
	void putteam(siz num, char team);

public:

	TeamBalancer(const RCon& rcon)
	: rcon(rcon), policy(TeamPolicy::create()) {}

	/**
	 * Get the current state of the server to reflect in the game object.
	 *
	 * @return True on success, false on failure.
	 */
	bool get_snapshot()
	{
		// GET guid's

		g.R.clear();
		g.B.clear();
		g.S.clear();
		g.players.clear();

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

			if(!sgl(sgl(siss(line) >> num >> team, skip, '('), guid, ')'))
			{
				log("ERROR: parsing !listplayers: " << line);
				return false;
			}

//			if(guid == "*") // bots present abort
//				return false;

			g.players[num].num = num;
			g.players[num].guid = guid;

			if(team == "R")
				g.R.insert(num);
			else if(team == "B")
				g.B.insert(num);
			else if(team == "S")
				g.S.insert(num);
		}
		// parse this info
//		con(response);

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

//		str prev_line[2]; // TODO: DEBUG LINE
		while(sgl(iss, line) && !trim(line).empty())
		{
//			player p;
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

//			if(ip == "bot") // bots present abort
//				return false;

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
	 * Query the server over rcon for the value of the cvar 'rconteam_policy'
	 * and select the appropriate TeamPolicy inplementation accordingly.
	 */
	void select_policy()
	{
		str response;
		if(!rcon.call("rconteam_policy", response))
		{
			log("WARN: rcon failure");
			return;
		}

		// Possible responses:
		// -> unknown command: rconteam_policy
		// -> "rconteam_policy" is:"FIFO^7", the default

		str policy_name;

		if(response.find("unknown command:"))
		{
			str skip;
			if(!sgl(sgl(siss(response), skip, ':').ignore(), policy_name, '^'))
				log("ERROR: parsing policy response: " << response);
		}

		con("pol: " << policy_name);

		if(!policy.get() || policy_name != policy->name())
			policy = TeamPolicy::create(policy_name);
	}

	void run()
	{

		while(!done)
		{
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
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
};

void TeamBalancer::call_teams(siz num, char team)
{
	// rcon chat "teams!!"
	rcon.call("chat ^3PLEASE BALANCE THE TEAMS!!");
	log("call_teams    : " << num << " " << g.players[num].name);
	++actions[std::to_string(num) + team]; // escalate
}

void TeamBalancer::request_player(siz num, char team)
{
	// rcon chat players[num].name please change to
	rcon.call("chat " + g.players[num].name + " ^3PLEASE CHANGE TEAMS!!");
	log("request_player: " << num << " " << g.players[num].name);
	++actions[std::to_string(num) + team]; // escalate
}

void TeamBalancer::putteam(siz num, char team)
{
	// rcon !putteam
//	rcon.call("chat ^3SORRY " + g.players[num].name + " BUT THE TEAMS NEED BALANCING");
	log("putteam       : " << num << " " << g.players[num].name);
	//rcon.call("chat !putteam " + num + " " + team);
	actions.clear(); // reset all players
}

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		con("usage: rconteam <host> <port> <rconpassword>");
		return -1;
	}

	siz port;
	if(!(siss(argv[2]) >> port))
	{
		con("error: bad port number: " << argv[2]);
		con("usage: rconteam <host> <port> <rconpassword>");
		return -2;
	}

	str host = argv[1];
	str pass = argv[3];

	RCon rcon(host, port, pass);

	// TODO: Check using rcon if server is running CTF game (gametype = 4?)

	TeamBalancer tb(rcon);
	tb.run();
}
