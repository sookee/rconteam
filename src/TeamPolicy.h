//#pragma once
#ifndef _OA_TEAMPOLICY_H__
#define _OA_TEAMPOLICY_H__

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

#include <memory>
#include <map>

#include "rcon.h"
#include "types.h"

namespace oa {

class TeamPolicy;

typedef std::shared_ptr<TeamPolicy> TeamPolicySPtr;
typedef std::map<str, TeamPolicySPtr> policy_map;

class TeamPolicy
{
private:
	static policy_map policies;

public:
	TeamPolicy();
	virtual ~TeamPolicy() {}

	/**
	 * Policy name
	 * @return Name of the implementing policy.
	 */
	virtual str name() const = 0;

	/**
	 * Make exactly one balancing suggestion to correct the teams.
	 * This function will be called repeatedly to discover
	 * a balancing action if necessary.
	 * @param g The game to be balanced.
	 * @param num Output the slot of the player to be moved.
	 * @return true if a balancing recommendation has been made else false.
	 * The return parameter num conteain the resommendation.
	 */
	virtual bool balance(const game& g, slot& num, team& t) = 0;

	/**
	 * Factory funstion for selecting policies
	 * @param type Type of policy to create.
	 * @return The selected policy or a default if type is not known.
	 */
	static TeamPolicySPtr create(const str& type = "");

	/**
	 * Get a str_set of available policy names.
	 * @return An str_set of available policies.
	 */
	static str_set get_policy_names();

	/**
	 * Get the default policy name.
	 * @return The default policy name
	 */
	static str get_default_policy_name();


	/**
	 * Get the name of the policy that does nothing.
	 * @return The null policy name
	 */
	static str get_null_policy_name();

	/**
	 * Register a policy with to make it
	 * available from the factory methods.
	 * @param policy The TeamPolicy implementation to
	 * be registered.
	 * @return true on success, else false
	 */
	static bool register_policy(TeamPolicy* policy);
};

} // oa

#endif /* _OA_TEAMPOLICY_H__ */
