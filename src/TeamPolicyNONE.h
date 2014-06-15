//#pragma once
#ifndef _OA_TEAMPOLICY_NONE_H__
#define _OA_TEAMPOLICY_NONE_H__

/*
 * TeamPolicyNONE.h
 *
 *  Created on: Jun 15, 2014
 *      Author: SooKee oasookee@gmail.com
 */

/*-----------------------------------------------------------------.
| Copyright (C) 2014 SooKee oaskivvy@gmail.com                     |
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

extern const str POLICY_NONE;

/**
 * Do nothng
 */
class TeamPolicyNONE
: public TeamPolicy
{
public:
	virtual str name() const;
	virtual bool balance(const game& g, siz& num, team_id& team);
};

} // oa

#endif /* _OA_TEAMPOLICY_NONE_H__ */
