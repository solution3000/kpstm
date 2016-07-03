#include "Geometry.hpp"

void CGeometry::compute()
{
	Matrix<double, 3, 3> A;
	Vector3d B;
	//计算(x,y)->(cdp,line) 映射关系
	A <<
		p1_.x_, p1_.y_, 1.0,
		p2_.x_, p2_.y_, 1.0,
		p3_.x_, p3_.y_, 1.0;

	B << p1_.cdp_, p2_.cdp_, p3_.cdp_;
	matxy2sx_.row(0)= A.colPivHouseholderQr().solve(B);
	B << p1_.line_, p2_.line_, p3_.line_;
	matxy2sx_.row(1)= A.colPivHouseholderQr().solve(B);

	//计算(cdp,line)->(x,y) 映射关系
	A <<
		p1_.cdp_, p1_.line_, 1.0,
		p2_.cdp_, p2_.line_, 1.0,
		p3_.cdp_, p3_.line_, 1.0;
	B << p1_.x_, p2_.x_, p3_.x_;
	matsx2xy_.row(0) = A.colPivHouseholderQr().solve(B);
	B << p1_.y_, p2_.y_, p3_.y_;
	matsx2xy_.row(1) = A.colPivHouseholderQr().solve(B);

}

void CGeometry::xy2sx(double x, double y, double &cdp, double &line)
{

}

void CGeometry::sx2xy(double cdp, double line, double &x, double &y)
{

}