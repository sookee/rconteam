/*
 * TeamPolicyFILO.cpp
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

#include "TeamPolicyLIFO.h"
#include "types.h"
#include "bug.h"
#include "log.h"

namespace oa {

const str POLICY_LIFO = "LIFO";

str TeamPolicyLIFO::name() const { return POLICY_LIFO; }

bool TeamPolicyLIFO::balance(const game& g, slot& num, team& t)
{
	const siz reds = g.teams.at(team::R).size();
	const siz blues = g.teams.at(team::B).size();

	const team& to = blues < reds ? team::B : team::R;
	const team& from = reds < blues ? team::B : team::R;

	if(g.teams.at(from).size() - g.teams.at(to).size() < 2)
		return false; // balanced

	log("blance: Teams need balancing");

	hr_time_point last_time;
	slot last_num = slot::null;
	bool found = false;
	for(const slot& this_num: g.teams.at(from))
	{
		if(g.players.at(this_num).joined <= last_time)
			continue;
		last_time = g.players.at(this_num).joined;
		last_num = this_num;
		found = true;
	}

	t = to;
	num = last_num;

	return found;
}

} // oa
