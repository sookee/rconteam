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

#include "log.h"
#include "rcon.h"
#include "types.h"
#include "TeamPolicy.h"

#include <thread>
#include <chrono>

using namespace oa;

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

	void call_teams(const str& guid, char team);
	void request_player(const str& guid, char team);
	void putteam(const str& guid, char team);

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
		// TODO: Use rcon() to populate the game object
		//
		// That means populating the player map {guid -> player} with
		// all the players found on the server and populating the teams
		// R,B and S with the guids if players on RED, BLUE or SPEC respectively.
		//
		// USE:
		//      str response;
		//      if(!rcon("status", response))
		//          return false;
		//
		//      // parse response here for player info
		//
		//      // ALSO will need this to get guids
		//
		//      if(!rcon("!listplayers", response))
		//          return false;
		//
		//      // parse response here for player info

		// GET guid's/names

		str response;
		if(!rcon.call("!listplayers", response))
			return false;

		// parse this info
		con(response);

		// GET teams/scores - neet to match to guids/names

		if(!rcon.call("status", response))
			return false;

		// parse this info
		con(response);

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
			str guid;
			char team;
			if(policy.get() && policy->action(g, guid, team))
			{
				if(actions[guid + team] == ACT_CALL_TEAMS)
					call_teams(guid, team);
				else if(actions[guid + team] == ACT_REQUEST_PLAYER)
					request_player(guid, team);
				else if(actions[guid + team] == ACT_PUTTEAM)
					putteam(guid, team);
			}

			// sleep for a bit
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
};

void TeamBalancer::call_teams(const str& guid, char team)
{
	// rcon chat "teams!!"
	rcon.call("chat ^3PLEASE BALANCE THE TEAMS!!");
	++actions[guid + team]; // escalate
}

void TeamBalancer::request_player(const str& guid, char team)
{
	// rcon chat players[guid].name please change to
	rcon.call("chat " + g.players[guid].name + " ^3PLEASE CHANGE TEAMS!!");
	++actions[guid + team]; // escalate
}

void TeamBalancer::putteam(const str& guid, char team)
{
	// rcon !putteam
	rcon.call("chat ^3SORRY " + g.players[guid].name + " BUT THE TEAMS NEED BALANCING");
	//rcon.call("chat !putteam " + guid + " " + team);
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
	TeamBalancer tb(rcon);
	tb.run();
}
