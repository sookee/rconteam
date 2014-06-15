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

#include "TeamPolicyNONE.h"
#include "TeamPolicyLIFO.h"
#include "TeamPolicySCORE.h"
#include "TeamPolicySKILL.h"

namespace oa {

const str POLICY_DEFAULT = POLICY_LIFO;

policy_map TeamPolicy::policies;

TeamPolicy::TeamPolicy()
{
}

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

str TeamPolicy::get_null_policy_name()
{
	return POLICY_NONE;
}

TeamPolicySPtr TeamPolicy::create(const str& type)
{
	if(policies.empty())
	{
		// Built-in policies
		register_policy(new TeamPolicyLIFO);
		register_policy(new TeamPolicySCORE);
		register_policy(new TeamPolicySKILL);
	}

	if(policies.find(type) != policies.cend())
		return policies.at(type);

	log("WARN: Unknown team policy: '" << type << "', using default");
	return policies.at(get_default_policy_name());
}

bool TeamPolicy::register_policy(TeamPolicy* policy)
{
	if(!policy)
	{
		log("NULL policy");
		return false;
	}
	policies[policy->name()] = TeamPolicySPtr(policy);
	return true;
}

} // oa
