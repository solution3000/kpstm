#pragma once

/*
观测系统映射类
*/

struct CGeometry
{
	CGeometry();
	~CGeometry();
public:
/*
	给定四个点: P1,P2, P3, P4
	就能确定正向和反向映射关系:
	(x,y) ->(cdp, line)
	(cdp,line) ->(x,y)
	用仿射矩阵表示:
	[
	M11, M12, M13;
	M21, M22, M23
	];
	大地坐标 映射 观测坐标   (x,y)->(cdp,line) :
	    cdp=M11*x+M12*y+M13;
	    line=M21*x+M22*y+M23;
    同理：
	观测坐标 映射 大地坐标(cdp,line)->(x,y):
	x=M11*cdp+M12*line+M13;
	y=M21*cdp+M22*line+M23;

	所以总体有六个未知数，3个点正好6个方程

	内部计算都采用double, 保证精度！
	(x,y)->(cdp,line)映射：
	A=[
	    x1 y1 1
		x2 y2 1
		x3 y3 1
	  ]
    X1=[M11 M12 M13];
	B1=[cdp1 cdp2 cdp3]
	A*X1=B1
	X2=[M21 M22 M23];
	B2=[line1 line2 line3];
	A*X2=B2
	(cdp,line)->(x,y)映射：
	A=[
	   cdp1 line1 1
	   cdp2 line2 1 
	   cdp3 line3 1
	   ]
	X1=[M11 M12 M13];
	B1=[x1 x2 x3]
	A*X1=B1
	X2=[M21 M22 M23]
	B2=[y1 y2 y3]
	A*X2=B2
*/
	struct Point
	{
		template<class T1, T2, T3, T4> Point(T1 x, T2 y, T3 cdp, T4 line)
		{
			x_ = x;
			y_ = y;
			cdp_ = cdp;
			line_ = line;
		}

		double x_;     //大地坐标x
		double y_;     //大地坐标y

		double cdp_;   //cdp坐标
		double line_;  //线坐标
	};
public:

	float matxy2sx_; // (x,y)->(cdp,line)
	float matsx2xy_;  //(cdp,line)->(x,y)

};

