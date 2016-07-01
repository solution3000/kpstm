
#include "Test.hpp"

int main(int argc, char **argv)
{
	//CTest::test_mpi(argc, argv);
	CTest::test_boost_filesystem(argc, argv);
	CTest::test_boost_file_mapping(argc, argv);
	CTest::test_grid(argc, argv);
	return 0;
}