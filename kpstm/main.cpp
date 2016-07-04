
#include "Test.hpp"
//#pragma comment(lib, "libglog_static.lib") 

int 
main(int argc, char **argv)
{
	//CTest::test_mpi(argc, argv);
	//CTest::test_boost_filesystem(argc, argv);
	//CTest::test_boost_file_mapping(argc, argv);
	//CTest::test_grid(argc, argv);
	//CTest::test_geometry(argc, argv);
	
	CTest::test_glog(argc, argv);

	return 0;
}