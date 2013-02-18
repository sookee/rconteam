/*
 * rconteam.cpp
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

#include <thread>
#include <chrono>

#include "log.h"
#include "str.h"
#include "rcon.h"
#include "types.h"
#include "TeamPolicy.h"
#include "TeamBalancer.h"

using namespace oa;

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		con("usage: rconteam <host> <port> <rconpassword>");
		return -1;
	}

	siz port;
	if(!(siss(argv[2]) >> port))
	{
		con("error: bad port number: " << argv[2]);
		con("usage: rconteam <host> <port> <rconpassword>");
		return -2;
	}

	str host = argv[1];
	str pass = argv[3];

	RCon rcon(host, port, pass);

	// TODO: Check using rcon if server is running CTF game (gametype = 4?)

	TeamBalancer tb(rcon);
	tb.run();
}
