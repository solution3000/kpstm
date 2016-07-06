#include "Test.hpp"


CTest::CTest()
{
}


CTest::~CTest()
{
}

/*
二维网格(nx,ny),nnode个节点，
求最优二维划分！
*/
static void good_partation(int nx, int ny, int nnode, int &nnode_x, int &nnode_y)
{
#define CEIL(n,m) ((n+m-1)/m*m)
	int good = 1;
	int nx_new, ny_new;
	nx_new = CEIL(nx, good);
	ny_new = CEIL(ny, nnode);
	int total = nx_new*ny_new;

	for (int k = 1; k <=nnode; k++)
	{
		if (nnode%k==0)
		{
			int i = k;
			int j = nnode / k;
			//扩张后的网格点,xy方向的网格数调整后恰好是对应节点数的整数倍
			nx_new=CEIL(nx, i);
			ny_new=CEIL(ny, j);
			int t = nx_new*ny_new;
			if (total >t)
			{
				total = t;
				good = k;
			}
		}
	}

	nnode_x = good;
	nnode_y = nnode / good;
	
	CHECK_EQ(nnode, nnode_x*nnode_y);

	return;

#undef CEIL
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

// http://mpi.deino.net/mpi_functions/index.htm 
// http://mpitutorial.com/tutorials/
// http://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/


void CTest::test_mpi_file_iread(int argc, char **argv)
{
	int myid, numprocs;
	int namelen;

	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Get_processor_name(processor_name, &namelen);

	//fprintf(stderr, "Hello World! Process %d of %d on %s\n", myid, numprocs, processor_name);

	char *filename = "f:/file.txt";
	MPI_File fh;

	CHECK(MPI_SUCCESS==MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY,
		MPI_INFO_NULL, &fh))
		<<"MPI_File open failed";

	char buf[32];
	
	//non-blocked read
	MPI_Request req;
	CHECK(MPI_SUCCESS==MPI_File_iread(fh, buf, 31, MPI_BYTE, &req))
		<<"MPI_File_iread failed";

	//wait for read 
	MPI_Status status;
	CHECK(MPI_SUCCESS==MPI_Wait(&req, &status))
		<<"MPI_Wait failed";
	
	/*
	typedef struct _MPI_Status {
		int count;
		int cancelled;
		int MPI_SOURCE;
		int MPI_TAG;
		int MPI_ERROR;
	} MPI_Status, *PMPI_Status;
	*/
	
	/*
	  MPI_Wait函数没有置 status.MPI_ERROR
	*/

	int nbytes;
	CHECK(MPI_SUCCESS==MPI_Get_count(&status, MPI_BYTE, &nbytes))
		<<"MPI_Get_count failed";

	buf[nbytes] = 0;

	using namespace std;
	using namespace boost;
	cout << format("nbytes=%d")%nbytes << endl;
	cout << buf << endl;
	
	CHECK(MPI_SUCCESS==MPI_File_close(&fh))
		<<"MPI_File_close failed";
	
	MPI_Finalize();
}



/*
测试多个MPI 节点，并发读取一个本地数据，
本地数据是一个大文件的一部分，期间所有的数据会通过MPI通信，
在所有节点之间遍历一次(仅遍历一次):
如果有N个节点，相当于有N个硬盘的RAID0, 读盘的同时，网络也在
交换数据！ 平均IO所用时间：t=O(T/N)， T为一个节点读取整个大文件的时间
通过nonblock IO, 计算会和IO overlapped, IO时间会被掩盖
*/
struct MsgHD
{
	uint16_t  flags;  //flags=1, 磁盘读写已经完成
	uint16_t  loops;  //记录数据包经过节点的数目, loops==nnode-1, 数据包不会发送给next节点
	int nrec;
	char addr[0];
};

typedef float rec_t[1024]; //记录长度
const static int NREC_PER_IO = 1024; //每次IO的REC数目

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

void CTest::test_mpi_global_file_ring(int argc, char **argv)
{
	using namespace std;
	using namespace boost;

	size_t msg_len = sizeof(MsgHD) + sizeof(rec_t)*NREC_PER_IO;
	
	LOG(INFO) << format("Sizeof(MsgHD)=%d") % (int)sizeof(MsgHD);
	LOG(INFO) << format("msg_len = %d") % (int)msg_len;

	
	//初始化缓冲区
	MsgHD *disk_bufs[2];
	MsgHD *recv_bufs[2];
	char *buf_base = new char[4 * msg_len];
	disk_bufs[0] = reinterpret_cast<MsgHD*>(buf_base);
	disk_bufs[1] = reinterpret_cast<MsgHD*>(buf_base + msg_len);
	recv_bufs[0] = reinterpret_cast<MsgHD*>(buf_base + 2 * msg_len);
	recv_bufs[1] = reinterpret_cast<MsgHD*>(buf_base + 3 * msg_len);
	int disk_current = 0;
	int recv_current = 0;

	MsgHD *compute_and_send_buf = 0;
	MPI_Request reqs[2];
	/*********************************/

	//初始化工作
	int myid, nnode;
	int prev, next;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nnode);
	prev = myid - 1;
	next = myid + 1;
	prev = (prev + nnode) % nnode;
	next = next %nnode;

	char *filename_base = "f:/file";
	char filename[1024];
	sprintf_s(filename, "%s.%d", filename_base, myid);
	
	MPI_File fh;

	goto J200;

	//每个节点生成一个磁盘文件
	CHECK(MPI_SUCCESS==MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh))
		<<"MPI_File_open failed";
	
	float *wbuf = (float*)disk_bufs[0]->addr;
	
	int total_rec = (110 + myid * 10)*NREC_PER_IO + 10;
	LOG(INFO) << format("%d: total_rec=%d") % myid%total_rec;

	for (int i = 0; i < total_rec; i++)
	{
		int n = sizeof(rec_t) / sizeof(float);
		
		for (int j = 0; j < n; j++)
		{
			wbuf[j] = 2000 * (myid+1) + i;
		}

		MPI_Status status;
		CHECK(MPI_SUCCESS == MPI_File_write(fh, wbuf, sizeof(rec_t), MPI_BYTE, &status))
			<< "MPI_File_write failed";

		int count;
		CHECK(MPI_SUCCESS == MPI_Get_count(&status, MPI_BYTE, &count))
			<<"MPI_Get_count failed";

		CHECK(count == sizeof(rec_t)) << "file_write failed";

		LOG_EVERY_N(INFO, 100) <<format("%d, i=%d")%myid%i;
	}
	
	CHECK(MPI_SUCCESS == MPI_File_close(&fh))
		<< "MPI_File_close failed";


	goto J100;


J200:

	//======================================================================================================

	CHECK(MPI_SUCCESS == MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh))
		<< "MPI_File_open failed";

	LOG(INFO) << "After MPI_File_open " << filename;

	int nbytes = sizeof(rec_t)*NREC_PER_IO;


	/*
	顺序很重要，一定irecv在iread前面：iread是数据源，irecv是接受者,否者会死锁
	*/

	//发起异步网络接收数据
	CHECK(MPI_SUCCESS == MPI_Irecv(recv_bufs[recv_current], msg_len, MPI_BYTE, prev, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[0]))
		<< "MPI_Irecv failed";
	//发起异步磁盘读数据
	CHECK(MPI_SUCCESS==MPI_File_iread(fh, disk_bufs[disk_current]->addr, nbytes, MPI_BYTE, &reqs[1]))
		<<"MPI_File_iread failed";

	int total_finished = 0;  //结束标识
	int recv_counts[100] = { 0 }; //统计信息

	LOG(INFO) << "Enter do{}while";
	do
	{
		int which = -1;
		MPI_Status status;
		
		/*
		MPI_Waitany 应该是按次序查询的，顺序很重要！
		*/
		CHECK(MPI_SUCCESS==MPI_Waitany(2, reqs, &which, &status))
			<<"MPI_Waitany failed";
		int tag;
		if (which == 0) //处理网络数据
		{
			tag = status.MPI_TAG;

			compute_and_send_buf = recv_bufs[recv_current];
			compute_and_send_buf->loops++;

			recv_counts[tag] += compute_and_send_buf->nrec;

			recv_current = (recv_current + 1) % 2;
			//发起异步网络接收数据
			CHECK(MPI_SUCCESS == MPI_Irecv(recv_bufs[recv_current], msg_len, MPI_BYTE, prev, MPI_ANY_TAG, MPI_COMM_WORLD, &reqs[0]))
					<< "MPI_Irecv failed";
		}
		else if (which == 1) //处理磁盘IO
		{
			tag = myid;

			compute_and_send_buf = disk_bufs[disk_current];

			int count;
			CHECK(MPI_SUCCESS == MPI_Get_count(&status, MPI_BYTE, &count))
				<< "MPI_Get_count failed";
			
			CHECK(count%sizeof(rec_t) == 0);

			compute_and_send_buf->nrec = count / sizeof(rec_t);
			compute_and_send_buf->loops= 1;

			recv_counts[tag] += compute_and_send_buf->nrec;
			
			//文件尾部处理
			if (count == nbytes) 
			{
				
				compute_and_send_buf->flags = 0;
				disk_current = (disk_current + 1) % 2;
				//发起异步磁盘读数据
				CHECK(MPI_SUCCESS == MPI_File_iread(fh, disk_bufs[disk_current]->addr, nbytes, MPI_BYTE, &reqs[1]))
					<< "MPI_File_iread failed";
			}
			else
			{
				compute_and_send_buf->flags = 1;
			}
		}
		MPI_Request send_req;
		MPI_Status  send_status;

		if (compute_and_send_buf->loops!=nnode)
		{
			//MPI_Send(compute_and_send_buf, msg_len, MPI_BYTE, next, tag, MPI_COMM_WORLD);
			CHECK(MPI_SUCCESS==MPI_Isend(compute_and_send_buf, msg_len, MPI_BYTE, next, tag, MPI_COMM_WORLD, &send_req))
				<<"MPI_Isend failed";
		}

		//模拟计算函数
		//do_compute(compute_and_send_buf);
		boost::this_thread::sleep(boost::posix_time::milliseconds(50 + 50 * myid));


		if (compute_and_send_buf->loops != nnode)
		{
			CHECK(MPI_SUCCESS == MPI_Wait(&send_req, &send_status))
				<< "MPI_Wait failed";
		}

		if (compute_and_send_buf->flags == 1)
		{
			total_finished++;
		}

	} while (total_finished != nnode);

	//if (myid == 0)
	LOG(INFO)<< format("%d: recv_counts={%d,%d,%d,%d}") % myid
		% recv_counts[0]
		% recv_counts[1]
		% recv_counts[2]
		% recv_counts[3] << endl;
	
	//取消irecv req
	MPI_Cancel(&reqs[0]);
	MPI_File_close(&fh);

J100:
	MPI_Finalize();
	delete[]buf_base;
}

/*
ROOT读取文件，然后通过ring接力的方式将数据广播给后面的节点
*/

void CTest::test_mpi_ring_bcast_file(int argc, char **argv)
{

	using namespace std;
	using namespace boost;

	static const int TAG_NORM = 0;      //正常数据
	static const int TAG_EOF  = 1;       //end of file!
	static const int TAG_BARRIER = 2;   //需要进行同步MPI_Barrier

	MPI_Init(&argc, &argv);

	int myid, nnode;
	int prev, next;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nnode);
	prev = myid - 1;
	next = myid + 1;
	if (prev < 0) prev = MPI_PROC_NULL;
	if (next == nnode) next = MPI_PROC_NULL;

	size_t msg_len = sizeof(MsgHD) + sizeof(rec_t)*NREC_PER_IO;
	int nbytes = sizeof(rec_t)*NREC_PER_IO;


	LOG(INFO) << format("Sizeof(MsgHD)=%d") % (int)sizeof(MsgHD);
	LOG(INFO) << format("msg_len = %d") % (int)msg_len;


	//初始化缓冲区, double buffers
	MsgHD *disk_bufs[2];  //只有root节点需要
	MsgHD *recv_bufs[2];
	char *buf_base = new char[4 * msg_len];
	disk_bufs[0] = reinterpret_cast<MsgHD*>(buf_base);
	disk_bufs[1] = reinterpret_cast<MsgHD*>(buf_base + msg_len);
	recv_bufs[0] = reinterpret_cast<MsgHD*>(buf_base + 2 * msg_len);
	recv_bufs[1] = reinterpret_cast<MsgHD*>(buf_base + 3 * msg_len);
	int disk_current = 0;
	int recv_current = 0;


	char *filename = "f:/file.0";

	MPI_File fh=MPI_FILE_NULL;
	MPI_Request disk_req, recv_req, send_req;

	if (myid == 0){ //root 读打开文件读数据
		CHECK(MPI_SUCCESS == MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh))
			<< "MPI_File_open failed";

		//发起异步READ
		CHECK(MPI_SUCCESS == MPI_File_iread(fh, disk_bufs[disk_current]->addr, nbytes, MPI_BYTE, &disk_req))
			<< "MPI_File_iread failed";
	}
	else //其他节点接收，转发
	{
		//发起异步RECV
		CHECK(MPI_SUCCESS == MPI_Irecv(recv_bufs[recv_current], msg_len, MPI_BYTE, prev, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_req))
			<< "MPI_Irecv failed";
	}
	int loops = 0;
	int recv_counts = 0;
	while (1)
	{
		if (myid == 0) //读数据，计算，转发
		{
			MPI_Status status;
			CHECK(MPI_SUCCESS==MPI_Wait(&disk_req, &status))
				<<"MPI_Wait failed";
			
			int tag = TAG_NORM;

			int count = 0;
			CHECK(MPI_SUCCESS==MPI_Get_count(&status, MPI_BYTE, &count))
				<<"MPI_Get_count failed";

			CHECK(count%sizeof(rec_t) == 0);

			MsgHD *sbuf = disk_bufs[disk_current];
			
			disk_current = (disk_current + 1) % 2;
			//发起异步READ
			CHECK(MPI_SUCCESS == MPI_File_iread(fh, disk_bufs[disk_current]->addr, nbytes, MPI_BYTE, &disk_req))
				<< "MPI_File_iread failed";

			sbuf->flags = 0;
			sbuf->loops = 1;
			sbuf->nrec = count / sizeof(rec_t);
			
			cerr << format("#0: loops: %d, nrec=%d") % loops%sbuf->nrec << endl;

			recv_counts += sbuf->nrec;

			
			//文件结束标志
			if (count < nbytes)
			{
				tag=TAG_EOF;
			}

			//触发检查点条件
			if (loops % 10 == 0)
			{
				tag += TAG_BARRIER;
			}
	
			CHECK(MPI_SUCCESS==MPI_Isend(sbuf, msg_len, MPI_BYTE, next, tag, MPI_COMM_WORLD, &send_req))
				<<"MPI_Isend failed";

			//模拟计算函数
			boost::this_thread::sleep(boost::posix_time::milliseconds(50 + 50 * myid));

			CHECK(MPI_SUCCESS==MPI_Wait(&send_req, &status))
				<<"MPI_Wait failed";


			if (tag&TAG_BARRIER)
			{
				CHECK(MPI_SUCCESS==MPI_Barrier(MPI_COMM_WORLD))
					<<"MPI_Barrier failed";
				//do_checkpoint();
				LOG(INFO) << format("Barrier: recv_counts: %d") % recv_counts << endl;
			}

			float *p = (float*)sbuf->addr;
			LOG(INFO) << format("loops: %d, float: %g") % loops%p[0] << endl;

			loops++;

			if (tag&TAG_EOF)
			{
				CHECK(MPI_SUCCESS==MPI_Cancel(&disk_req))
					<<"MPI_Cancel failed";
				CHECK(MPI_SUCCESS==MPI_File_close(&fh))
					<<"MPI_File_close failed";
				break;
			}

		}
		else //接收，计算，转发
		{
			MPI_Status status;
			CHECK(MPI_SUCCESS==MPI_Wait(&recv_req, &status))
				<<"MPI_Wait failed";

			MsgHD *rbuf = recv_bufs[recv_current];
			
			recv_counts += rbuf->nrec;
			
			//发起异步RECV
			recv_current = (recv_current + 1) % 2;
			CHECK(MPI_SUCCESS==MPI_Irecv(recv_bufs[recv_current], msg_len, MPI_BYTE, prev, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_req))
				<<"MPI_Irecv failed";

			int tag = status.MPI_TAG;
			//转发给下一个NODE
			CHECK(MPI_SUCCESS==MPI_Isend(rbuf, msg_len, MPI_BYTE, next, tag, MPI_COMM_WORLD, &send_req))
				<<"MPI_Isend failed";

			//模拟计算函数
			boost::this_thread::sleep(boost::posix_time::milliseconds(50 + 50 * myid));


			CHECK(MPI_SUCCESS==MPI_Wait(&send_req, &status))
				<<"MPI_Wait failed";

			if (tag&TAG_BARRIER)
			{
				CHECK(MPI_SUCCESS == MPI_Barrier(MPI_COMM_WORLD)) 
					<< "MPI_Barrier failed";
				//检查点工作
				//do_checkpoint()
				LOG(INFO) << format("Barrier: recv_counts: %d") % recv_counts << endl;
			}

			float *p = (float*)rbuf->addr;
			LOG(INFO) << format("loops: %d, float: %g") % loops%p[0] << endl;

			loops++;


			if (tag&TAG_EOF)
			{
				//退出
				CHECK(MPI_SUCCESS==MPI_Cancel(&recv_req))
					<<"MPI_Cancel failed";
				break;
			}
			

		}//if(myid==0)

	}//while(1)

	CHECK(MPI_SUCCESS==MPI_Barrier(MPI_COMM_WORLD))
		<<"MPI_Barrier failed";
	
	LOG(INFO) << format("Finished, recv_counts:%d") %recv_counts<< endl;

	delete[]buf_base;
	MPI_Finalize();

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
	CHECK_DOUBLE_EQ(geo.Mxy2sx_(0, 0), 0.038452031536422);
	CHECK_DOUBLE_EQ(geo.Mxy2sx_(1, 0), -0.011019910713241);


	cout << geo.Msx2xy_ << endl;
	//24.032200357781754   -6.888719512195123  35580.3387925957
	//6.887298747763370    24.032012195122075  623960.2851100615
	CHECK_DOUBLE_EQ(geo.Msx2xy_(0, 0), 24.032200357781754);
	CHECK_DOUBLE_EQ(geo.Msx2xy_(1, 0), 6.887298747763370);

	double x, y, line, cdp;
	geo.xy2sx(geo.p4_.x_, geo.p4_.y_, line, cdp);
	LOG(INFO)<<format("(line,cdp)=(%g,%g)") % line%cdp<<endl;
	CHECK_DOUBLE_EQ(line, geo.p4_.line_);
	CHECK_DOUBLE_EQ(cdp, geo.p4_.cdp_);

	geo.sx2xy(geo.p4_.line_, geo.p4_.cdp_, x, y);
	LOG(INFO)<< format("(x,y)=(%g,%g)") % x%y << endl;
	CHECK_DOUBLE_EQ(x, geo.p4_.x_);
	CHECK_DOUBLE_EQ(y, geo.p4_.y_);

	line = 1255;
	cdp = 5955;

	geo.sx2xy(line, cdp, x, y);
	LOG(INFO)<< format("(x,y)=(%g,%g)") % x%y << endl;

}

// 如何使用glog:  http://www.programgo.com/article/7481399830/;jsessionid=1AC8ADAE01EC6F1B7EBBBD5AD12B5722

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

//vs2013报错，对C++11支持不好
// https://github.com/nlohmann/json
//#include <json.hpp>

void CTest::test_card(int argc, char **argv)
{
	//json内不能有注释
	//CCard::test("./c.json");
}



void CTest::run(int argc, char **argv)
{

	test_glog(argc, argv);

	//test_mpi(argc, argv);
	//test_boost_filesystem(argc, argv);
	//test_boost_file_mapping(argc, argv);
	//test_grid(argc, argv);
	//test_geometry(argc, argv);
	//test_card(argc, argv);
	
	//test_mpi_file_iread(argc, argv);
	//test_mpi_global_file_ring(argc, argv);
	test_mpi_ring_bcast_file(argc, argv);

}
