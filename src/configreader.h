#pragma once
#include "types.h"
#include "debug.h"
#include <iostream>
#include "libs/rapidxml-1.13/rapidxml.hpp"

class dbConfig
{

	public:
		void getDatabaseConfig( oa::str xml_configfilename , oa::dbinfo* conf );

	private:

};

