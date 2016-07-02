#pragma once
#include <tuple>

class CGrid
{
public:
	template <class T>
	bool check(T x0, T x1, T dx)
	{
		if (dx == 0) return false;
		if (dx > 0 && x1 < x0) return false;
		if (dx<0 && x1>x0) return false;

		return true;
	}
};
/*
1D网格：{x0:dx:x1}, [matlab notation]
设定之后只读，不准更改！
*/

template<class T>
class  CGrid1D : CGrid
{
public:
	explicit CGrid1D(T x0, T x1, T dx)
	{
		if (!check(x0, x1, dx)){

		}

		mx_ = std::make_tuple(x0, x1, dx);
	}
public:
	T x0() const { return std::get<0>(mx_); }
	T x1() const { return std::get<1>(mx_); }
	T dx() const { return  std::get<2>(mx_); }
	int nx() const { return (x1() - x0()) / dx() + 1; }
private:
	//T &x0() { return  std::get<0>(mx_); }
	//T &x1(){ return   std::get<1>(mx_); }
	//T &dx() { return  std::get<2>(mx_); }
private:
	std::tuple<T, T, T> mx_;
};

/*
2D 网格{x0:dx:x1}*{y0:dy:y1}
设定之后只读，不准更改！
*/

template<class T>
class CGrid2D : CGrid
{
public:
	explicit CGrid2D(T x0, T x1, T dx,
		T y0, T y1, T dy)
	{
		if (!(check(x0, x1, dx) && check(y0, y1, dy)))
		{

		}
		mx_ = std::make_tuple(x0, x1, dx);
		my_ = std::make_tuple(y0, y1, dy);
	}
public:
	T x0() const { return  std::get<0>(mx_); }
	T x1() const { return  std::get<1>(mx_); }
	T dx() const{ return   std::get<2>(mx_); }
	int nx() const{ return (x1() - x0()) / dx() + 1; }

private:
	//T &x0() { return  std::get<0>(mx_); }
	//T &x1(){ return   std::get<1>(mx_); }
	//T &dx() { return  std::get<2>(mx_); }

public:
	T y0() const { return  std::get<0>(my_); }
	T y1() const { return  std::get<1>(my_); }
	T dy() const{ return   std::get<2>(my_); }
	int ny() const{ return (y1() - y0()) / dy() + 1; }
private:
	//T &y0() { return  std::get<0>(my_); }
	//T &y1(){ return   std::get<1>(my_); }
	//T &dy() { return  std::get<2>(my_); }

private:
	std::tuple<T, T, T> mx_, my_;
};


/*
3D 网格: {x0:dx:x1} * {y0:dy:y1} * {z0:dz:z1}
设定之后只读，不准更改！
*/
template<class T>
class CGrid3D : CGrid
{
public:
	explicit CGrid3D(T x0, T x1, T dx,
		T y0, T y1, T dy,
		T z0, T z1, T dz)
	{
		if (!(check(x0, x1, dx) && check(y0, y1, dy) && check(z0, z1, dz)))
		{

		}
		mx_ = std::make_tuple(x0, x1, dx);
		my_ = std::make_tuple(y0, y1, dy);
		mz_ = std::make_tuple(z0, z1, dz);
	}
public:
	T x0() const { return  std::get<0>(mx_); }
	T x1() const { return  std::get<1>(mx_); }
	T dx() const{ return   std::get<2>(mx_); }
	int nx() const{ return (x1() - x0()) / dx() + 1; }

private:
	//T &x0() { return  std::get<0>(mx_); }
	//T &x1(){ return   std::get<1>(mx_); }
	//T &dx() { return  std::get<2>(mx_); }

public:
	T y0() const { return  std::get<0>(my_); }
	T y1() const { return  std::get<1>(my_); }
	T dy() const{ return   std::get<2>(my_); }
	int ny() const{ return (y1() - y0()) / dy() + 1; }
private:
	//T &y0() { return  std::get<0>(my_); }
	//T &y1(){ return   std::get<1>(my_); }
	//T &dy() { return  std::get<2>(my_); }

public:
	T z0() const { return  std::get<0>(mz_); }
	T z1() const { return  std::get<1>(mz_); }
	T dz() const{ return   std::get<2>(mz_); }
	int nz() const{ return (z1() - z0()) / dz() + 1; }

private:
	//T &z0() { return  std::get<0>(mz_); }
	//T &z1(){ return   std::get<1>(mz_); }
	//T &dz() { return  std::get<2>(mz_); }

private:
	std::tuple<T, T, T> mx_, my_, mz_;
};
