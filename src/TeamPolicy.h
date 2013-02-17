#pragma once
#ifndef TEAMPOLICY_H_
#define TEAMPOLICY_H_

/*
 * TeamPolicy.h
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

#include "types.h"

namespace oa {

class TeamPolicy
{
public:
	TeamPolicy() {}
	virtual ~TeamPolicy() {}

	virtual bool balance() = 0;
};

class LIFOTeamPolicy
: public TeamPolicy
{
public:

	/**
	 * rearrang the players to produce a balanced
	 * game according to the implementing policy
	 * @param g The game to be balanced.
	 * @return true, if changes were made, else false
	 */
	virtual bool balance(game& g);
};

} // oa

#endif /* TEAMPOLICY_H_ */