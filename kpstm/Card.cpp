#include "Card.hpp"


void CCard::test(const char *cardfile)
{
	using namespace std;
	using namespace boost::property_tree;

	try{
		ptree pt;
		read_json(cardfile, pt);
	}
	catch (exception const &e)
	{
		cerr << e.what() << endl;
	}
}
