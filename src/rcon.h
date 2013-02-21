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

#include <memory>

namespace oa {

class RCon
{
protected:
	mutable str res;

public:
	virtual ~RCon() {}

	virtual bool call(const str& cmd, str& res) const = 0;

	bool call(const str& cmd) const
	{
		return call(cmd, res);
	}
};

class RConImpl
: public RCon
{
	const str host;
	const siz port;
	const str pass;

	bool rcon(const str& host, siz port, const str& cmd, str& reply) const;

public:
	RConImpl(const str& host, siz port, const str& pass)
	: host(host), port(port), pass(pass) {}

	virtual bool call(const str& cmd, str& res) const
	{
		return rcon(host, port, "rcon " + pass + " " + cmd, res);
	}
};

class RConStream
: public RCon
{
private:
	std::istream& i;
	std::ostream& o;

public:
	RConStream(std::istream& i, std::ostream& o): i(i), o(o) {}

	virtual bool call(const str& cmd, str& res) const
	{
		res.clear();
		o << "rcon " + cmd << std::endl;
		str line;
		while(&res != &this->res && sgl(i, line) && !line.empty())
			res += line + '\n';
		return i;
	}
};

typedef std::shared_ptr<RCon> RConSPtr;

} // oa

#endif // _OA_RCON_H_
