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

<<<<<<< HEAD
int main( int argc , char* arguments[] )
{
	return -1;
=======
#include "log.h"
#include "rcon.h"
#include "types.h"

#include <thread>
#include <chrono>

using namespace oa;

class TeamBalancer
{
private:
	const str host;
	const siz port;
	const str pass;

	bool done = false;

	game g; // current snapshot

public:

	TeamBalancer(const str& host, siz port, const str& pass)
	: host(host), port(port), pass(pass) {}

	/**
	 * Get the current state of the server to reflect in the game object.
	 *
	 * @return True on success, false on failure.
	 */
	bool get_snapshot()
	{
		// TODO: Use rcon() to populate the game object
		//
		// That means populating the player map {guid -> player} with
		// all the players found on the server and populating the teams
		// R,B and S with the guids if players on RED, BLUE or SPEC respectively.
		//
		// USE:
		//      str response;
		//      if(!rcon(host, port, "rcon <password> status", response))
		//          return false;
		//
		//      // parse response here for player info
		//
		//      // ALSO will need this to get guids
		//
		//      if(!rcon(host, port, "rcon <password> !listplayers", response))
		//          return false;
		//
		//      // parse response here for player info

		// GET guid's/names

		str response;
		if(!rcon(host, port, "rcon " + pass + " !listplayers", response))
			return false;

		// parse this info
		con(response);

		// GET teams/scores - neet to match to guids/names

		if(!rcon(host, port, "rcon " + pass + " status", response))
			return false;

		// parse this info
		con(response);

		return true;
	}

	void run()
	{

		while(!done)
		{
			get_snapshot();
			g.dump(std::cout); // for now
			// update game snapshot from rcon
			// select team policy (rcon variable)
			// calculate new team
			// implement team chages using rcon
			// -> request / !putteam
			// sleep for a bit
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
};

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

	TeamBalancer tb(host, port, pass);
	tb.run();
>>>>>>> 8ac6d500ab27d8d35d86911d48ee955c7b3e7360
}
