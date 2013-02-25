/*
 * TeamPolicySKILL.cpp
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

#include "TeamPolicySKILL.h"

#include "types.h"
#include "bug.h"
#include "log.h"

namespace oa {

str TeamPolicySKILL::name() const { return POLICY_SKILL; }

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

bool TeamPolicySKILL::balance(const game& g, siz& num, team_id& team)
{
	const siz reds = g.teams.at(team_id::R).size();
	const siz blues = g.teams.at(team_id::B).size();

	const team_id& to = blues < reds ? team_id::B : team_id::R;
	const team_id& from = reds < blues ? team_id::B : team_id::R;

	const siz num_of_changes = g.teams.at(from).size() - g.teams.at(to).size();

	// get stats from previous snapshots for this map
	stat_map stats; // guid -> stat

	// cache relevant info
	std::map<siz, str> num_guids; // num -> guid
	std::map<str, siz> guid_nums; // guid -> num

	str_set guids;
	for(const siz n: g.teams.at(team_id::R))
	{
		if(g.players.find(n) == g.players.cend())
			continue;
		guids.insert(g.players.at(n).guid);
		num_guids[n] = g.players.at(n).guid;
		guid_nums[g.players.at(n).guid] = n;
	}
	for(const siz n: g.teams.at(team_id::B))
	{
		if(g.players.find(n) == g.players.cend())
			continue;
		guids.insert(g.players.at(n).guid);
		num_guids[n] = g.players.at(n).guid;
		guid_nums[g.players.at(n).guid] = n;
	}

	read_stats(g.mapname, guids, stats);

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
	update_stats(g.mapname, stats);

	if(num_of_changes < 2)
		return false; // balanced

	log("blance: Teams need balancing");

	siz s = 0;

	siz to_ave = 0; // average of to team average scores
	if((s = g.teams.at(to).size()) > 0)
	{
		for(const siz n: g.teams.at(to))
			to_ave += stats[num_guids[n]].average_score;
		to_ave /= s;
	}

	siz from_ave = 0; // average of to team average scores
	if((s = g.teams.at(to).size()) > 0)
	{
		for(const siz n: g.teams.at(from))
			from_ave += stats[num_guids[n]].average_score;
		from_ave /= s;
	}

	siz low = siz(-1);
	siz change_num = siz(-1);
	if(to_ave >= from_ave) // pick weakest player to move
	{
		for(const siz n: g.teams.at(from))
		{
			if(stats[num_guids[n]].average_score < low)
			{
				low = stats[num_guids[n]].average_score;
				change_num = n;
			}
		}
	}
	else // pick to make closest skill match
	{
		// only try to make up for one player's worth of difference
		const siz diff_ave = (from_ave - to_ave) / num_of_changes;
		const siz ideal_ave = (to_ave + diff_ave) / 2;

		for(const siz n: g.teams.at(from))
		{
			// calculate projected new to_ave
			siz new_to_ave = ((to_ave * g.teams.at(to).size())
				+ stats[num_guids[n]].average_score) / (g.teams.at(to).size() + 1);

			if(new_to_ave < ideal_ave)
			{
				if(ideal_ave - new_to_ave < low)
				{
					low = ideal_ave - new_to_ave;
					change_num = n;
				}
			}
			else
			{
				if(new_to_ave - ideal_ave < low)
				{
					low = new_to_ave - ideal_ave;
					change_num = n;
				}
			}
		}
	}

	if(change_num == siz(-1))
		log("ERROR: failed to find lowest average score");
	else
		num = change_num;

	return true;
}

} // oa
