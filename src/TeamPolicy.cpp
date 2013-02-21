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
#include "bug.h"
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

	log("blance: Teams need balancing");

	hr_time_point last_time;
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

struct stat
{
	siz snapshots = 0;
	siz average_score = 0;
};

typedef std::map<str, stat> stat_map; // guid -> stat

/**
 * Accumulate game score stats in stat_map.
 * @param mapname
 * @param guids
 * @param stats
 */
void read_stats(const str& mapname, const str_set& guids, stat_map& stats)
{
	// <guid> <mapname> <snapshots> <average-score>
	stats.clear();
	std::ifstream ifs("SkillTeamPolicy-stats.txt");
	str line;
	str g, m;
	stat st;
	while(sgl(ifs, line))
	{
		if(!(siss(line) >> g >> m >> st.snapshots >> st.average_score))
			continue;
		if(m != mapname || guids.find(g) == guids.end())
			continue;
		stats.at(g) = st;
	}
}

void update_stats(const str& mapname, const stat_map& stats)
{
	std::ifstream ifs("SkillTeamPolicy-stats.txt");
	sss ss;
	str line;
	str g, m;
	stat st;
	while(sgl(ifs, line))
	{
		if(!(siss(line) >> g >> m >> st.snapshots >> st.average_score))
			continue;
		if(m != mapname || stats.find(g) == stats.end())
		{
			ss << line << '\n';
			continue;
		}
		ss << g << ' ' << mapname
			<< ' ' << stats.at(g).snapshots
			<< ' ' << stats.at(g).average_score << '\n';
	}
	ifs.close();
	std::ofstream ofs("SkillTeamPolicy-stats.txt");
	while(sgl(ss, line))
		ofs << line << '\n';
}

/**
 * Ballance number of players by switching
 * last the most appropriately skilled person.
 * @param g
 */
bool SkillTeamPolicy::balance(const game& g, siz& num, team_id& team)
{
	const siz reds = g.teams.at(team_id::R).size();
	const siz blues = g.teams.at(team_id::B).size();

	const team_id& to = blues < reds ? team_id::B : team_id::R;
	const team_id& from = reds < blues ? team_id::B : team_id::R;

	const siz num_of_changes = g.teams[from].size() - g.teams[to].size();

	if(num_of_changes < 2)
		return false; // balanced

	log("blance: Teams need balancing");


	// get stats from previous snapshots for this map
	stat_map stats; // guid -> stat

	// cache relevant info
	std::map<siz, str> num_guids; // num -> guid
	std::map<str, siz> guid_nums; // guid -> num

	str_set guids;
	for(const siz n: g.teams[team_id::R])
	{
		if(g.players.find(n) == g.players.cend())
			continue;
		guids.insert(g.players.at(n).guid);
		num_guids[n] = g.players.at(n).guid;
		guid_nums[g.players.at(n).guid] = n;
	}
	for(const siz n: g.teams[team_id::B])
	{
		if(g.players.find(n) == g.players.cend())
			continue;
		guids.insert(g.players.at(n).guid);
		num_guids[n] = g.players.at(n).guid;
		guid_nums[g.players.at(n).guid] = n;
	}

	read_stats(g.map, guids, stats);

	// update the stats from the snapshot g
	for(auto& s: stats)
	{
		const siz n = guid_nums[s.first];
		if(g.players.find(n) == g.players.cend())
			continue;
		siz snapshot = s.second.snapshots + 1;
		s.second.average_score = ((s.second.average_score * s.second.snapshots)
			+ g.players.at(n).score) / snapshot;
		s.second.snapshots = snapshot;
	}

	// store stats updated from game snapshot
	update_stats(g.map, stats);

	siz s = 0;
	siz to_ave = 0; // average of to team average scores
	if((s = g.teams[to].size()) > 0)
	{
		for(const siz n: g.teams[to])
			to_ave += stats[num_guids[n]].average_score;
		to_ave /= s;
	}
	siz from_ave = 0; // average of to team average scores
	if((s = g.teams[to].size()) > 0)
	{
		for(const siz n: g.teams[from])
			from_ave += stats[num_guids[n]].average_score;
		from_ave /= s;
	}

	siz change_num = siz(-1);
	if(to_ave >= from_ave) // pick weakest player to move
	{
		siz ave = siz(-1);
		for(const siz n: g.teams[from])
		{
			if(stats[num_guids[n]].average_score < ave)
			{
				ave = stats[num_guids[n]].average_score;
				change_num = n;
			}
		}
	}
	else // pick to make closest skill match
	{
		// only try to make up for one player's worth of difference
		const siz diff_ave = (from_ave - to_ave) / num_of_changes;
		const siz ideal_ave = (to_ave + diff_ave) / 2;

		siz diff = siz(-1);;
		for(const siz n: g.teams[from])
		{
			// calculate projected new to_ave
			siz new_to_ave = ((to_ave * g.teams[to].size())
				+ stats[num_guids[n]].average_score) / (g.teams[to].size() + 1);

			if(new_to_ave < ideal_ave)
			{
				if(ideal_ave - new_to_ave < diff)
				{
					diff = ideal_ave - new_to_ave;
					change_num = n;
				}
			}
			else
			{
				if(new_to_ave - ideal_ave < diff)
				{
					diff = new_to_ave - ideal_ave;
					change_num = n;
				}
			}
		}
	}
	if(change_num == siz(-1))
		log("ERROR: failed to find lowest average score");
	else
		num = change_num;

	log("ERROR: This TeamPolicy implementation is unfinished");
	return true;
}

} // oa
