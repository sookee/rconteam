#pragma once
#ifndef _OA_RCON_H_
#define _OA_RCON_H_

/*
 * rcon.h
 *
 *  Created on: 01 June 2012
 *      Author: oasookee@googlemail.com
 */

/*-----------------------------------------------------------------.
| Copyright (C) 2012 SooKee oasookee@googlemail.com               |
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

class RCon
{
	const str host;
	const siz port;
	const str pass;

	bool rcon(const str& host, int port, const str& cmd, str& res) const;

public:
	RCon(const str& host, siz port, const str& pass)
	: host(host), port(port), pass(pass) {}

	bool call(const str& cmd, str& res) const
	{
		return rcon(host, port, "rcon " + pass + " " + cmd, res);
	}

	bool call(const str& cmd) const
	{
		str res;
		return call(cmd, res);
	}
};

} // oa

#endif // _OA_RCON_H_
