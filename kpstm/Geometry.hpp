#pragma once

/*
�۲�ϵͳӳ����
*/

struct CGeometry
{
	CGeometry();
	~CGeometry();
public:
/*
	�����ĸ���: P1,P2, P3, P4
	����ȷ������ͷ���ӳ���ϵ:
	(x,y) ->(cdp, line)
	(cdp,line) ->(x,y)
	�÷�������ʾ:
	[
	M11, M12, M13;
	M21, M22, M23
	];
	������� ӳ�� �۲�����   (x,y)->(cdp,line) :
	    cdp=M11*x+M12*y+M13;
	    line=M21*x+M22*y+M23;
    ͬ��
	�۲����� ӳ�� �������(cdp,line)->(x,y):
	x=M11*cdp+M12*line+M13;
	y=M21*cdp+M22*line+M23;

	��������������δ֪����3��������6������

	�ڲ����㶼����double, ��֤���ȣ�
	(x,y)->(cdp,line)ӳ�䣺
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
	(cdp,line)->(x,y)ӳ�䣺
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

		double x_;     //�������x
		double y_;     //�������y

		double cdp_;   //cdp����
		double line_;  //������
	};
public:

	float matxy2sx_; // (x,y)->(cdp,line)
	float matsx2xy_;  //(cdp,line)->(x,y)

};

