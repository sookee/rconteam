#pragma once
#ifndef _OA_TYPES_H_
#define _OA_ TYPES_H_

/*
 * types.h
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

namespace oa {

typedef std::string str;
typedef std::size_t siz;

typedef std::set<str> str_set;

struct player
{
	str guid;
	siz score;
	std::time_t joined;
};

typedef str_set team;

struct game
{
	// guids
	team R;
	team B;

	std::map<str, player> players;
};

} // oa

#endif /* TEAMPOLICY_H_ */
