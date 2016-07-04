#pragma once
/*
作业卡读取类
*/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <string>
#include <sstream>


class CCard
{
public:
	CCard();
	virtual ~CCard()=0;
public:
	bool parse_input();  //输入参数
	bool parse_geo();    //观测系统参数
	bool parse_output(); //输出参数
	bool parse_ckpt();   //checkpoint参数
	bool parse_mig();    //偏移控制参数
	bool parse_vel();    //速度场参数
public:
	static void test(const char *cardfile);
};

/*时间偏移作业卡*/
class CCardTime : public CCard
{
public:
	CCardTime();
	virtual ~CCardTime();
};

/*深度偏移作业卡*/
class CCardDepth : public CCard
{
public:
	CCardDepth();
	virtual ~CCardDepth();
};