/*
 * TeamPolicy.cpp
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

//#include <cmath>

#include "TeamPolicy.h"
#include "types.h"
#include "log.h"

namespace oa {

const str POLICY_LIFO = "LIFO";
const str POLICY_SKILL = "SKILL";
const str POLICY_DEFAULT = POLICY_LIFO;

static const std::map<str, TeamPolicySPtr> policies =
{
	{POLICY_LIFO, TeamPolicySPtr(new LIFOTeamPolicy)}
	, {POLICY_SKILL, TeamPolicySPtr(new SkillTeamPolicy)}
};

str_set TeamPolicy::get_policy_names()
{
	str_set keys;
	for(const auto& p: policies)
		keys.insert(p.first);
	return keys;
}

str TeamPolicy::get_default_policy_name()
{
	return POLICY_DEFAULT;
}

TeamPolicySPtr TeamPolicy::create(const str& type)
{
	if(policies.find(type) != policies.cend())
		return policies.at(type);

	log("WARN: Unknown team policy:" << type << ", using default");
	return policies.at(POLICY_DEFAULT);
}

//bool TeamPolicy::action(const game& g, siz& num, const team_id& team)
//{
//	if(std::abs(int(g.teams.at(team_id::B).size()) - int(g.teams.at(team_id::R).size())) < 2)
//		return false; // ballanced
//
//	const siz reds = g.teams[team_id::R].size();
//	const siz blues = g.teams[team_id::B].size();
//	const team_id& to = blues < reds ? team_id::B : team_id::R;
//	const team_id& from = reds < blues ? team_id::B : team_id::R;
//
//	if(!balance(g, from, to, num))
//		return false;
//	t = g.B.size() < g.R.size() ? 'B' : 'R';
//	return true;
//}

str LIFOTeamPolicy::name() const { return POLICY_LIFO; }

/**
 * Ballance number of players by switching
 * last person to join first.
 * @param g
 */
bool LIFOTeamPolicy::balance(const game& g, siz& num, team_id& team)
{
	const siz reds = g.teams.at(team_id::R).size();
	const siz blues = g.teams.at(team_id::B).size();

	const team_id& to = blues < reds ? team_id::B : team_id::R;
	const team_id& from = reds < blues ? team_id::B : team_id::R;

	if(g.teams[from].size() - g.teams[to].size() < 2)
		return false; // balanced

	hr_time_point last_time = hr_clk::now();
	siz last_num = 0;
	bool found = false;
	for(const siz& this_num: g.teams.at(from))
	{
		if(g.players.at(this_num).joined <= last_time)
			continue;
		last_time = g.players.at(this_num).joined;
		last_num = this_num;
		found = true;
	}

	team = to;

	num = last_num;
	return found;
}

str SkillTeamPolicy::name() const { return POLICY_SKILL; }

/**
 * Ballance number of players by switching
 * last the most appropriately skilled person.
 * @param g
 */
bool SkillTeamPolicy::balance(const game& g, siz& num, team_id& team)
{
	bool changed = false;
	// TODO: Implement this
	log("ERROR: This TeamPolicy implement is unfinished");
	return changed;
}

} // oa
