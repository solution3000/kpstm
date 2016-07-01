#pragma once
#include <mpi.h>
#include <stdio.h>



class CTest
{
public:
	CTest();
	~CTest();
public:
	static void test_mpi(int argc, char **argv);
};

