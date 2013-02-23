#pragma once
#ifndef _OA_TEAMPOLICY_SKILL_H__
#define _OA_TEAMPOLICY_SKILL_H__

/*
 * TeamPolicy.h
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

#include <memory>

#include "TeamPolicy.h"

#include "rcon.h"
#include "types.h"

namespace oa {

const str POLICY_SKILL = "SKILL";

/**
 * Ballance number of players by switching
 * the most appropriately skilled person.
 * This policy stores player statistics between
 * games based on the player's GUID.
 */
class TeamPolicySKILL
: public TeamPolicy
{
public:
	virtual str name() const;
	virtual bool balance(const game& g, siz& num, team_id& team);
};

} // oa

#endif /* _OA_TEAMPOLICY_SKILL_H__ */
