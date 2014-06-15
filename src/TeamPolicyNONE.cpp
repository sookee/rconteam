/*
 * TeamPolicyNONE.cpp
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

//#include <cmath>

#include "TeamPolicyNONE.h"
#include "types.h"
#include "bug.h"
#include "log.h"

namespace oa {

const str POLICY_NONE = "NONE";

str TeamPolicyNONE::name() const { return POLICY_NONE; }

bool TeamPolicyNONE::balance(const game& g, siz& num, team_id& team)
{
	return false;
}

} // oa
