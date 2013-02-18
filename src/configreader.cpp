#include "configreader.h"

using namespace std;
using namespace oa;


/** read the configuration for the db
* from an XML file into a container object to pass around.
* @param xml_configFileName the filename of the config 
* @param conf the container object in which the data is passed around
* TODO: check if file exists, check if opperation is succesfull (XML may be not valid)
*
* EXAMPLE on how to use:
* oa::dbinfo* conf = new oa::dbinfo();
* dbConfig* obj = new dbConfig();
* obj->getDatabaseConfig( "config.xml" , conf );
* //conf contains all the data now!
*/
void dbConfig::getDatabaseConfig( str xml_configFileName , oa::dbinfo* conf )
{
	STUB_WARNING("Check TODO for function!");	

	std::ifstream myfile( xml_configFileName );
	rapidxml::xml_document<> doc;

	/* "Read file into vector<char>*/
	vector<char> buffer((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
   	buffer.push_back('\0');

   	//cout<<&buffer[0]<<endl; //test the buffer
	
	doc.parse<0>(&buffer[0]);  


	/*read into container object here*/
        rapidxml::xml_node<> *node = doc.first_node("dbhostname");
	conf->host = node->value();

	node = doc.first_node("dbport");
	conf->port = node->value();

	node = doc.first_node("dblogin");
	conf->login = node->value();

	node = doc.first_node("dbpassword");
	conf->password = node->value();

	node = doc.first_node("dbname");
	conf->dnName = node->value();
	
}

