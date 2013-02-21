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
		con("usage: rconteam -test <input file> <output file>");
		return -1;
	}

	RConSPtr rcon;

	std::ifstream ifs;
	std::ofstream ofs;

	siz delay = 10;

	if(str(argv[1]) == "-test")
	{
		ifs.open(argv[2]);
		if(!ifs)
		{
			con("ERROR: opening input file: " << argv[2]);
			return -2;
		}

		ofs.open(argv[3]);
		if(!ofs)
		{
			con("ERROR: opening output file: " << argv[3]);
			return -3;
		}

		rcon.reset(new RConStream(ifs, ofs));
		delay = 2;
	}
	else
	{
		siz port;
		if(!(siss(argv[2]) >> port))
		{
			con("error: bad port number: " << argv[2]);
			con("usage: rconteam <host> <port> <rconpassword>");
			return -4;
		}

		str host = argv[1];
		str pass = argv[3];

		rcon.reset(new RConImpl(host, port, pass));
	}

	if(!rcon.get())
	{
		con("ERROR: null rcon");
		return -5;
	}

	TeamBalancer tb(rcon);
	tb.set_delay(delay);
	tb.run();
}
