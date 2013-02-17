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

#include "TeamPolicy.h"

#include <cmath>

namespace oa {

/**
 * Ballance number of players awitching
 * last person to join first.
 * @param g
 */
bool LIFOTeamPolicy::ballance(game& g)
{
	if(std::abs(int(g.B.size()) - int(g.R.size())) < 2)
		return true; // ballanced

	team& to = g.B.size() < g.R.size() ? g.B : g.R;
	team& from = g.R.size() < g.B.size() ? g.B : g.R;

	while(from.size() - to.size() > 1)
	{
		time_t last_time = time_t(-1);
		str last_guid;

		for(const str& guid: from)
		{
			if(g.players[guid].joined < last)
			{
				last_time = g.players[guid].joined;
				last_guid = guid;
			}
		}

		if(guid.empty())
		{
			// should never happen
			return false;
		}

		from.erase(last_guid);
		to.insert(last_guid);
	}

	return true;
}

} // oa
