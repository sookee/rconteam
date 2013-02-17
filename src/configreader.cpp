#include "configreader.h"

void dbConfig::getDatabaseConfig( oa::dbinfo* conf )
{
	STUB_WARNING( "Read database configuration from XML?" );
	conf->host = "localhost";
        conf->port = "";
        conf->login = "";
        conf->password = "";
        conf->dnName = "";
}

