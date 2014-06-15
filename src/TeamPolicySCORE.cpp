/*
 * TeamPolicySCORE.cpp
 *
 *  Created on: Feb 23, 2013
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

//#include <cmath>

#include "TeamPolicySCORE.h"
#include "types.h"
#include "bug.h"
#include "log.h"

namespace oa {

const str POLICY_SCORE = "SCORE";

str TeamPolicySCORE::name() const { return POLICY_SCORE; }

struct stat
{
	st_time_point score_reset;
	siz score_max; // maximum score since score_reset
};
typedef std::map<str, stat> stat_map;

struct rate
{
	std::chrono::minutes mins;
	siz score;
};
typedef std::map<str, rate> rate_map;

stat_map stats; // guid -> stat
rate_map rates; // guid -> rate

bool TeamPolicySCORE::balance(const game& g, siz& num, team_id& team)
{
	// update average scores

	for(auto& pp: g.players)
	{
		// delete missing players/specs from stats & rates
		if((!g.teams.at(team_id::R).count(pp.first) && !g.teams.at(team_id::B).count(pp.first))
		|| g.teams.at(team_id::S).count(pp.first))
		{
			stats.erase(pp.second.guid);
			rates.erase(pp.second.guid);
			continue;
		}

		// calc stats
		const player& p = pp.second;
		stat& s = stats[p.guid];
		rate& r = rates[p.guid];

		if(p.score >= s.score_max)
			s.score_max = p.score;
		else // store & reset
		{
			st_time_point now = st_clk::now();
			r.score = (r.score * r.mins.count()
				+ s.score_max)
				/ (r.mins.count()
				+ std::chrono::duration_cast<std::chrono::minutes>(now - s.score_reset).count());
			s.score_max = 0;
			s.score_reset = now;
		}
	}

	const siz reds = g.teams.at(team_id::R).size();
	const siz blues = g.teams.at(team_id::B).size();

	const team_id& to = blues < reds ? team_id::B : team_id::R;
	const team_id& from = reds < blues ? team_id::B : team_id::R;

	if(g.teams.at(from).size() - g.teams.at(to).size() < 2)
		return false; // balanced

	log("blance: Teams need balancing");

//	hr_time_point last_time;
//	siz last_num = 0;
//	bool found = false;
//	for(const siz& this_num: g.teams.at(from))
//	{
//		if(g.players.at(this_num).joined <= last_time)
//			continue;
//		last_time = g.players.at(this_num).joined;
//		last_num = this_num;
//		found = true;
//	}
//
//	team = to;
//	num = last_num;
//
	return true;
}

} // oa
