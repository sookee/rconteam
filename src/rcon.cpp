/*
 * rcon.cpp
 *
 *  Created on: 01 June 2012
 *      Author: oaskivvy@gmail.com
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

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <thread>
#include <chrono>
#include <sstream>
#include <map>
#include <memory>
#include <mutex>

#include "bug.h"
#include "log.h"
#include "types.h"
#include "rcon.h"

namespace oa {

bool RCon::rcon(const str& host, int port, const str& cmd, str& res) const
{
	bug_func();
	// One mutex per server:port to ensure that all threads
	// accessing the same server:port pause for a minimum time
	// between calls to avoid flood protection.
//	static std::map<str, std::unique_ptr<std::mutex>> mtxs;

	int cs = socket(PF_INET, SOCK_DGRAM, 0);

	if(cs <= 0)
	{
		log(strerror(errno));
		return false;
	}

	sockaddr_in sin;
	hostent* he = gethostbyname(host.c_str());
	std::copy(he->h_addr, he->h_addr + he->h_length
		, reinterpret_cast<char*>(&sin.sin_addr.s_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if(connect(cs, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) < 0)
	{
		log(strerror(errno));
		return false;
	}

	fcntl(cs, F_SETFL, O_NONBLOCK);

//	const str key = host + ":" + std::to_string(port);
//
//	if(!mtxs[key].get())
//		mtxs[key].reset(new std::mutex);

	// keep out all threads for the same server:port until the minimum time
	// has elapsed
//	lock_guard lock(*mtxs[key]);
//	st_time_point pause = st_clk::now() + std::chrono::milliseconds(1000);

	const str msg = "\xFF\xFF\xFF\xFF" + cmd;
	if(send(cs, msg.c_str(), msg.size(), 0) < 1)
	{
		log(strerror(errno));
		return false;
	}

	res.clear();

	int len;
	char buf[1024];
	for(siz i = 0; i < 10; ++i)
	{
		for(; (len = read(cs, buf, 1024)) > 9; i = 0)
			res.append(buf + 10, len - 10);
		if(len > 0)
			res.append(buf, len);
		else if(len == 0)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	close(cs);

//	std::this_thread::sleep_until(pause);

	return true;
}
#define TIMEOUT 1000
bool aocom(const str& cmd, str_vec& packets, const str& host, int port
	, siz wait = TIMEOUT)
{
	// One mutex per server:port to ensure that all threads
	// accessing the same server:port pause for a minimum time
	// between calls to avoid flood protection.
	static std::map<str, std::unique_ptr<std::mutex>> mtxs;

	int cs = socket(PF_INET, SOCK_DGRAM, 0);
	int cs_flags = fcntl(cs, F_GETFL, 0);

	if(cs <= 0)
	{
		log(strerror(errno));
		return false;
	}

	sockaddr_in sin;
	hostent* he = gethostbyname(host.c_str());
	std::copy(he->h_addr, he->h_addr + he->h_length
		, reinterpret_cast<char*>(&sin.sin_addr.s_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	fcntl(cs, F_SETFL, cs_flags | O_NONBLOCK);

	st_time_point timeout = st_clk::now() + std::chrono::milliseconds(wait);

	int n = 0;
	while((n = connect(cs, (struct sockaddr *) &sin, sizeof(sin))) < 0 && errno == EINPROGRESS)
	{
		if(st_clk::now() > timeout)
		{
			log("socket timed out connecting to: " << host << ":" << port);
			return false;
		}
		std::this_thread::yield();
	}

	if(n < 0)
	{
		log(strerror(errno) << ": " << host << ":" << port);
		return false;
	}

	fcntl(cs, F_SETFL, cs_flags);

	const str msg = "\xFF\xFF\xFF\xFF" + cmd;

	if((n = send(cs, msg.c_str(), msg.size(), 0)) < 0 || n < (int)msg.size())
	{
		log("cs send: " << strerror(errno));
		return false;
	}

	packets.clear();

	char buf[2048];

	n = sizeof(buf);
	while(n == sizeof(buf))
	{
//		timeout = st_clk::now() + std::chrono::milliseconds(wait);
		while((n = recv(cs, buf, sizeof(buf), MSG_DONTWAIT)) ==  -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		{
			if(st_clk::now() > timeout)
			{
				log("socket timed out connecting to: " << host << ":" << port);
				return false;
			}
			std::this_thread::yield();
		}
	//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if(n < 0)
			log("cs recv: " << strerror(errno));
		if(n > 0)
			packets.push_back(str(buf, n));
	}

	close(cs);

	return true;
}

bool RCon::getrcon(const str& host, siz port, const str& cmd, str& reply) const
{
	str_vec packets;
	if(!aocom(cmd, packets, host, port, TIMEOUT))
		return false;

	const str header = "\xFF\xFF\xFF\xFFprint\x0A";

	if(packets.empty())
	{
		log("Empty response.");
		return false;
	}

	reply.clear();
	for(const str& packet: packets)
	{
		if(packet.find(header) != 0)
		{
			log("Unrecognised response.");
			return false;
		}

		reply.append(packet.substr(header.size()));
	}

	return true;
}

} // oa

