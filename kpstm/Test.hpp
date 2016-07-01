#pragma once
#include <stdio.h>
#include <iostream>
#include <mpi.h>
#include<boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

class CTest
{
public:
	CTest();
	~CTest();
public:
	static void test_mpi(int argc, char **argv);
	static void test_boost_filesystem(int argc, char **argv);
	static void test_boost_file_mapping(int argc, char **argv);
};

