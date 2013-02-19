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

#include "log.h"
#include "rcon.h"
#include "types.h"
#include "TeamPolicy.h"

namespace oa {

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

	const RCon rcon;
	TeamPolicySPtr policy;

	game g; // current snapshot

	st_time_point pause;

	enum
	{
		ACT_CALL_TEAMS, ACT_REQUEST_PLAYER, ACT_PUTTEAM
	};

	typedef std::map<str, siz> action_map;
	action_map actions;

	void call_teams(siz num, char team);
	void request_player(siz num, char team);
	void putteam(siz num, char team);

	void chat(const str& text);

	template<typename T>
	void rconset(const str& cvar, T& val)
	{
		str response;
		if(!rcon.call(cvar, response))
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
	TeamBalancer(const RCon& rcon)
	: rcon(rcon), policy(TeamPolicy::create())
	, pause(st_clk::now() + std::chrono::seconds(10))
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

	void run();
};

} // oa

#endif /* _OA_TEAMBALANCER_H_ */
