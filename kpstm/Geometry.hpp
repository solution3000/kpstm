#pragma once

#include <iostream>
#include <Eigen/Dense>

/*
�۲�ϵͳӳ����
*/

/*
�����ĸ���: P1,P2, P3, P4
����ȷ������ͷ���ӳ���ϵ:
(x,y) ->(line, cdp)
(line, cdp) ->(x,y)
�÷�������ʾ:
[
M11, M12, M13;
M21, M22, M23
];
������� ӳ�� �۲�����   (x,y)->(line,cdp) :
cdp=M11*x+M12*y+M13;
line=M21*x+M22*y+M23;
ͬ��
�۲����� ӳ�� �������(line,cdp)->(x,y):
x=M11*cdp+M12*line+M13;
y=M21*cdp+M22*line+M23;

��������������δ֪����3��������6������

�ڲ����㶼����double, ��֤���ȣ�
(x,y)->(line,cdp)ӳ�䣺
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
(line,cdp)->(x,y)ӳ�䣺
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
using namespace Eigen;
struct CGeometry
{
public:
	CGeometry(
		double x1, double y1, double line1, double cdp1,   //��һ����
		double x2, double y2, double line2, double cdp2,   //�ڶ�����
		double x3, double y3, double line3, double cdp3,   //��������
		double x4, double y4, double line4,  double cdp4   //���ĸ��㣬������֤
		)
	{
		p1_ = Point(x1, y1, line1, cdp1);
		p2_ = Point(x2, y2, line2, cdp2);
		p3_ = Point(x3, y3, line3, cdp3);
		p4_ = Point(x4, y4, line4, cdp4);

		compute();

	}
	~CGeometry(){}
public:
	struct Point
	{
		Point(double x=0, double y=0, double line=0, double cdp=0)
		{
			x_ = x;
			y_ = y;
			line_ = line;
			cdp_ = cdp;
		}
		double x_;     //�������x
		double y_;     //�������y
		double line_;  //������
		double cdp_;   //cdp����
	};
public:
	Matrix<double, 2,3> Mxy2sx_; // (x,y)->(line,cdp)
	Matrix<double, 2,3> Msx2xy_;  //(line,cdp)->(x,y)
public:
	Point p1_, p2_, p3_, p4_;
public:
	//(x, y)->(line,cdp)
	void xy2sx(double x, double y, double &line, double &cdp);
	//(line, cdp)->(x,y)
	void sx2xy(double line, double cdp, double &x, double &y);
private:
	void compute();
};

//  http://www.geo.uib.no/eworkshop/nav-processing/geometry/
//  https://sourceforge.net/projects/seismic3dsurvey/ 
//  https://sourceforge.net/projects/botoseis/  
//  http://www.tenaxgeo.com/resources.html  free tool