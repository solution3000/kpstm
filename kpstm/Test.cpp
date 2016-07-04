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
	using namespace boost;


	CGrid1D<int> g1d_int(0, 10, 1);

	LOG(INFO) << "Grid1D:" << endl;
	LOG(INFO) << format("\t(x0,x1,dx,nx)=(%d,%d,%d,%d)") %
		g1d_int.x0() %
		g1d_int.x1() %
		g1d_int.dx() %
		g1d_int.nx() << endl;

	CGrid2D<float> g2d_float(
		0.0, 1.0, 0.1,
		1.0, 2.0, 0.3
		);

	LOG(INFO) << "Grid2D:" << endl;
	LOG(INFO) << format("\t(x0,x1,dx,nx)=(%g,%g,%g,%d)") %
		g2d_float.x0() %
		g2d_float.x1() %
		g2d_float.dx() %
		g2d_float.nx() << endl;

	LOG(INFO) << format("\t(y0,y1,dy,ny)=(%g,%g,%g,%d)") %
		g2d_float.y0() %
		g2d_float.y1() %
		g2d_float.dy() %
		g2d_float.ny() << endl;

	CGrid3D<float> g3d_float(
		0.0, 1.0, 0.5,
		1, 2, 0.2,
		3, 10, 0.3
		);

	LOG(INFO) << "Grid3D:" << endl;
	LOG(INFO) << format("\t(x0,x1,dx,nx)=(%g,%g,%g,%d)") %
		g3d_float.x0() %
		g3d_float.x1() %
		g3d_float.dx() %
		g3d_float.nx() << endl;
	
	LOG(INFO) << format("\t(y0,y1,dy,ny)=(%g,%g,%g,%d)") %
		g3d_float.y0() %
		g3d_float.y1() %
		g3d_float.dy() %
		g3d_float.ny() << endl;

	LOG(INFO) << format("\t(z0,z1,dz,nz)=(%g,%g,%g,%d)") %
		g3d_float.z0() %
		g3d_float.z1() %
		g3d_float.dz() %
		g3d_float.nz() << endl;

	return;
}

/*
342204.00 6380040.00 1025 5550
337685.00 6395805.00 1025 6206
351119.00 6399655.00 1584 6206
355638.00 6383890.00 1584 5550
*/


void CTest::test_geometry(int argc, char **argv)
{
	using namespace std;
	using namespace boost;

	CGeometry geo(
		342204.00, 6380040.00, 1025, 5550,
		337685.00, 6395805.00, 1025, 6206,
		351119.00, 6399655.00, 1584, 6206,
		355638.00, 6383890.00, 1584, 5550
		);
	
	cout << geo.Mxy2sx_ << endl;
	// 0.038452031536422   0.011022183984338   -82455.41370732605
	//-0.011019910713241   0.038452332603035  -236006.3625749549
	
	cout << geo.Msx2xy_ << endl;
	//24.032200357781754   -6.888719512195123  35580.3387925957
	//6.887298747763370    24.032012195122075  623960.2851100615

	double x, y, line, cdp;
	geo.xy2sx(geo.p4_.x_, geo.p4_.y_, line, cdp);
	LOG(INFO)<<format("(line,cdp)=(%g,%g)") % line%cdp<<endl;

	geo.sx2xy(geo.p4_.line_, geo.p4_.cdp_, x, y);
	LOG(INFO)<< format("(x,y)=(%g,%g)") % x%y << endl;

	line = 1255;
	cdp = 5955;

	geo.sx2xy(line, cdp, x, y);
	LOG(INFO)<< format("(x,y)=(%g,%g)") % x%y << endl;

}

void CTest::test_glog(int argc, char **argv)
{
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination(google::INFO, "./log.txt");

	LOG(INFO) << "FOUND" << google::COUNTER << std::endl;
	LOG(INFO) << "hello, world";
	LOG(WARNING) << "warning test";
	LOG(ERROR) << "error test";

	return;

}

void CTest::run(int argc, char **argv)
{

	CTest::test_glog(argc, argv);

	//CTest::test_mpi(argc, argv);
	//CTest::test_boost_filesystem(argc, argv);
	//CTest::test_boost_file_mapping(argc, argv);
	CTest::test_grid(argc, argv);
	CTest::test_geometry(argc, argv);
}
