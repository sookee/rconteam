/*
 * bug.cpp
 *
 *  Created on: 8 Feb 2013
 *      Author: oasookee@gmail.com
 */

/*-----------------------------------------------------------------.
| Copyright (C) 2013 SooKee oasookee@gmail.com                     |
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
#include "bug.h"

#include <thread>
#include <iostream>

#include <cstdio>
#include <execinfo.h>
#include <signal.h>
#include <cstdlib>
#include <cxxabi.h>

#include <memory>

namespace oa {

#define THREAD oa::thread_name()
#define OBJECT oa::obj_name(this)

str thread_name()
{
	std::thread::id id = std::this_thread::get_id();
	static std::map<std::thread::id, siz> thread_map;
	static size_t thread_count(0);

	if(thread_map.find(id) == thread_map.end())
	{
		thread_map[id] = thread_count++;
	}
	return "[" + std::to_string(thread_map[id]) + "]";
}

str obj_name(void* id)
{
	static std::map<void*, siz> obj_map;
	static siz obj_count(0);

	if(obj_map.find(id) == obj_map.end())
		obj_map[id] = obj_count++;

	std::ostringstream oss;
	oss << std::hex << id;
	return "{" + std::to_string(obj_map[id]) + ": " + oss.str() + "}";
}

std::ostream& botbug(std::ostream* os)
{
	static std::ostream* osp = 0;

	if(!osp) // initialize
		if(!os)
			osp = &std::cout;

	if(os) // change
		osp = os;

	return *osp;
}

str embellish(siz indent)
{
	return std::string(indent, '-');
}

__scope__bomb__::__scope__bomb__(const char* name): name(name)
{
	++indent;
	bug("");
	bug(str(indent, '-') + "> " << name << ' ' << THREAD << ' ' << OBJECT);
}
__scope__bomb__::~__scope__bomb__()
{
	bug("<" << str(indent, '-') << " " << name << ' ' << THREAD << ' ' << OBJECT);
	bug("");
	--indent;
}

size_t __scope__bomb__::indent = 0;

} // oa
