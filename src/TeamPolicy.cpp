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

namespace oa {

bool TeamPolicy::action(const game& g, siz& num, char& t)
{
	if(std::abs(int(g.B.size()) - int(g.R.size())) < 2)
		return false; // ballanced

	const team& to = g.B.size() < g.R.size() ? g.B : g.R;
	const team& from = g.R.size() < g.B.size() ? g.B : g.R;

	if(!balance(g, from, to, num))
		return false;
	t = g.B.size() < g.R.size() ? 'B' : 'R';
	return true;
}

/**
 * Ballance number of players by switching
 * last person to join first.
 * @param g
 */
bool LIFOTeamPolicy::balance(const game& g, const team& from, const team& to, siz& num)
{
	time_t last_time = time_t(0);
	siz last_num = 0;
	bool found = false;
	for(const siz& this_num: from)
	{
		if(g.players.at(this_num).joined > last_time)
		{
			last_time = g.players.at(this_num).joined;
			last_num = this_num;
			found = true;
		}
	}

	num = last_num;
	return found;
}

/**
 * Ballance number of players by switching
 * last the most appropriately skilled person.
 * @param g
 */
bool SkillTeamPolicy::balance(const game& g, const team& from, const team& to, siz& num)
{
	if(std::abs(int(g.B.size()) - int(g.R.size())) < 2)
		return false; // ballanced

	bool changed = false;
	while(from.size() - to.size() > 1)
	{
//		changed = true;
	}
	return changed;
}

TeamPolicySPtr TeamPolicy::create(const str& type)
{
	if(type == "LIFO")
		return TeamPolicySPtr(new LIFOTeamPolicy);
	else if(type == "SKILL")
		return TeamPolicySPtr(new SkillTeamPolicy);

	// default
	return TeamPolicySPtr(new LIFOTeamPolicy);
}


} // oa
