#include "Test.hpp"


CTest::CTest()
{
}


CTest::~CTest()
{
}

void CTest::test_mpi(int argc, char **argv)
{
	int myid, numprocs;
	int namelen;

	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Get_processor_name(processor_name, &namelen);

	fprintf(stderr, "Hello World! Process %d of %d on %s\n", myid, numprocs, processor_name);

	MPI_Finalize();
	return;
}

/*
测试的几个函数：
is_regular_file
file_size
space
temp_directory_path
copy
copy_file
resize_file
*/
void CTest::test_boost_filesystem(int argc, char **argv)
{
	using namespace boost::filesystem;
	using namespace std;
	
	auto ret =is_regular_file(argv[0]);
	auto fsize = file_size(argv[0]);
	
	cout << "ret=" << ret << endl;
	cout << "fsize=" << fsize << endl;
	

	//space 对输入比较苛刻
	//path p("C:\\Windows\\System");
	//auto disk_space = space(p);

	//argv[1] = "./" 可以， 
	cout << argv[1] << endl;
	auto disk_space = space(argv[1]);

	cout << "available : " <<disk_space.available << endl;
	cout << "capacity : " <<disk_space.capacity << endl;
	cout << "free : " <<disk_space.free << endl;

	cout << temp_directory_path() << endl;


}

/*

测试boost::interprocess的两个文件内存映射类：
	file_mapping
	mapped_region

*/

void CTest::test_boost_file_mapping(int argc, char **argv)
{

}