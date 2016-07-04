#include "Geometry.hpp"

void CGeometry::compute()
{
	Matrix<double, 3, 3> A;
	Vector3d B;
	//计算(x,y)->(line,cdp) 映射关系
	A <<
		p1_.x_, p1_.y_, 1.0,
		p2_.x_, p2_.y_, 1.0,
		p3_.x_, p3_.y_, 1.0;

	B << p1_.line_, p2_.line_, p3_.line_;
	Mxy2sx_.row(0) = A.colPivHouseholderQr().solve(B);

	B << p1_.cdp_, p2_.cdp_, p3_.cdp_;
	Mxy2sx_.row(1)= A.colPivHouseholderQr().solve(B);
	
	//计算(line,cdp,line)->(x,y) 映射关系
	A <<
		p1_.line_, p1_.cdp_, 1.0,
		p2_.line_, p2_.cdp_, 1.0,
		p3_.line_, p3_.cdp_, 1.0;
	B << p1_.x_, p2_.x_, p3_.x_;
	Msx2xy_.row(0) = A.colPivHouseholderQr().solve(B);
	B << p1_.y_, p2_.y_, p3_.y_;
	Msx2xy_.row(1) = A.colPivHouseholderQr().solve(B);

}

void CGeometry::xy2sx(double x, double y, double &line, double &cdp)
{
	line= Mxy2sx_(0, 0)*x + Mxy2sx_(0, 1)*y + Mxy2sx_(0, 2);
	cdp = Mxy2sx_(1, 0)*x + Mxy2sx_(1, 1)*y + Mxy2sx_(1, 2);
	return;
}

void CGeometry::sx2xy(double line, double cdp, double &x, double &y)
{
	x = Msx2xy_(0, 0)*line + Msx2xy_(0, 1)*cdp + Msx2xy_(0, 2);
	y = Msx2xy_(1, 0)*line + Msx2xy_(1, 1)*cdp + Msx2xy_(1, 2);
	return;
}