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
	using namespace boost::filesystem;
	using namespace boost::interprocess;
	using namespace std;

	path p("D:/tbb44.zip");

	//create file mappable object
	file_mapping mp(p.string().c_str(), read_write);

	//create mapped region from mappable object
	mapped_region region(mp, read_write);
	
	//madvise something
	region.advise(mapped_region::advice_willneed);

	cout << region.get_address() << endl;
	cout << region.get_size() << endl;
	cout << region.get_page_size() << endl;
	
	//destroy
	region.~mapped_region();
	
	return;
}

void CTest::test_grid(int argc, char **argv)
{
	using namespace std;

	CGrid1D<int> g1d_int(0, 10, 1);
	
	cout << "Grid1D:\n";
	cout << g1d_int.x0() << endl;
	cout << g1d_int.x1() << endl;
	cout << g1d_int.dx() << endl;
	cout << g1d_int.nx() << endl;


	CGrid2D<float> g2d_float(
		0.0, 1.0, 0.1,
		1.0, 2.0, 0.3
		);

	cout << "Grid2D:\n";
	cout << g2d_float.x0() << endl;
	cout << g2d_float.x1() << endl;
	cout << g2d_float.dx() << endl;
	cout << g2d_float.nx() << endl;

	cout << g2d_float.y0() << endl;
	cout << g2d_float.y1() << endl;
	cout << g2d_float.dy() << endl;
	cout << g2d_float.ny() << endl;

	CGrid3D<float> g3d_float(
		0.0, 1.0, 0.5,
		1, 2, 0.2,
		3, 10, 0.3
		);
	cout << "Grid3D:\n";
	cout << g3d_float.x0() << endl;
	cout << g3d_float.x1() << endl;
	cout << g3d_float.dx() << endl;
	cout << g3d_float.nx() << endl;

	cout << g3d_float.y0() << endl;
	cout << g3d_float.y1() << endl;
	cout << g3d_float.dy() << endl;
	cout << g3d_float.ny() << endl;

	cout << g3d_float.z0() << endl;
	cout << g3d_float.z1() << endl;
	cout << g3d_float.dz() << endl;
	cout << g3d_float.nz() << endl;

	return;
}