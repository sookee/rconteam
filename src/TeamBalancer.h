#pragma once
#ifndef _OA_TEAMBALANCER_H_
#define _OA_TEAMBALANCER_H_

/*
 * TeamBalancer.h
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

#include <memory>

#include "bug.h"
#include "log.h"
#include "rcon.h"
#include "types.h"
#include "TeamPolicy.h"

namespace oa {

/**
 * Using rcon the TeamBalancer monitors an
 * OpenArena server and issues recommendations and/or
 * moves players to keep the teams in balance.
 *
 * The team balance is determined by a TeamPolicy.
 */
class TeamBalancer
{
private:
	static const str prefix;
	static const str suffix;

	bool done = false;
	bool enforcing = false;
	bool verbose = false;
	bool testing = false;
	bool ignore_bots = false;
	bool team_game = false; // is it a team game? g_gametype = 4..

	RConSPtr rcon;
	TeamPolicySPtr policy;

	game g; // current snapshot

	/**
	 * Time between pauses.
	 */
	std::chrono::seconds delay;

	/**
	 * Waiting point between game snapshots.
	 */
	st_time_point pause;

	enum
	{
		ACT_CALL_TEAMS, ACT_REQUEST_PLAYER, ACT_PUTTEAM
	};

	/**
	 * Keep trach of how many warnings each player has had
	 * (by slot number <num>).
	 */
	typedef std::map<str, siz> action_map; // <num> -> int
	action_map actions;

	/**
	 * General message to all players to balance teams.
	 * @param num The player recommended to move.
	 * @param team The recommended team for the player to join.
	 */
	void call_teams(siz num, const team_id& team);

	/**
	 * Message to a specific player to balance teams.
	 * @param num The player recommended to move.
	 * @param team The recommended team for the player to join.
	 */
	void request_player(siz num, const team_id& team);

	/**
	 * If policy is ENFORCING then actuall move the specific player
	 * to the recommended team. Oterwise just request they change team.
	 * @param num The player recommended to move.
	 * @param team The recommended team for the player to join.
	 */
	void putteam(siz num, const team_id& team);

	/**
	 * Print a chat message on the console for all players to read.
	 * @param text The text to be displayed.
	 */
	void chat(const str& text) const;

	/**
	 * Print a text message only on the console of the specified player.
	 * @param num The slot number of the player to be addressed.
	 * @param text The text to be displayed to the specified player.
	 * TODO: Not working - simply calls chat(text);
	 */
	void tell(siz num, const str& text) const;

	/**
	 * Set a variable from a cvar using rcon.
	 * @param cvar The name of the cvar whose value is wanted
	 * @param val The variable to set to the cvar's value.
	 */
	template<typename T>
	void rconset(const str& cvar, T& val)
	{
		str response;
		if(!rcon->call(cvar, response))
		{
			log("WARN: rconset failure: " << cvar);
			return;
		}

		// Possible responses:
		// -> unknown command: <var>
		// -> "<var>" is:"<val>^7", the default

		str sval;

		if(response.find("unknown command:"))
		{
			str skip;
			if(!sgl(sgl(siss(response), skip, ':').ignore(), sval, '^'))
				log("ERROR: parsing policy response: " << response);
		}
		siss(sval) >> val;
	}

public:
	TeamBalancer(const RConSPtr& rcon)
	: rcon(rcon), policy(TeamPolicy::create())
	, delay(10)
	, pause(st_clk::now() + delay)
	{}

	/**
	 * Get the current state of the server to reflect in the game object.
	 *
	 * @return True on success, false on failure.
	 */
	bool get_snapshot();

	/**
	 * Query the server over rcon for the value of the cvar 'rconteam_policy'
	 * and select the appropriate TeamPolicy inplementation accordingly.
	 */
	void select_policy();

	/**
	 * The main processing loop. This function will not return
	 * until done == true.
	 */
	void run();

	void set_delay(siz secs)
	{
		delay = std::chrono::seconds(secs);
		pause = st_clk::now() + delay;
	}
};

} // oa

#endif /* _OA_TEAMBALANCER_H_ */
