
#include "Test.hpp"
//#define GOOGLE_GLOG_DLL_DECL

//#include <glog/logging.h>

int 
main(int argc, char **argv)
{
	//google::InitGoogleLogging(argv[0]);
	//google::SetLogDestination(google::INFO, "./log.txt");

	CTest::run(argc, argv);
	return 0;
}