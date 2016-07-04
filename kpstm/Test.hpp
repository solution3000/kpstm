#pragma once
#include <stdio.h>
#include <iostream>

#include <mpi.h>
#include<boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/format.hpp>

#include <glog/logging.h>

#include "Grid.hpp"
#include "Geometry.hpp"
#include "Card.hpp"



class CTest
{
public:
	CTest();
	~CTest();
public:
	static void run(int argc, char **argv);
public:
	static void test_mpi(int argc, char **argv);
public:
	static void test_boost_filesystem(int argc, char **argv);
	static void test_boost_file_mapping(int argc, char **argv);
	static void test_glog(int argc, char **argv);
public:
	static void test_boost_format(int argc, char **argv);
public:
	static void test_grid(int argc, char **argv);
	static void test_geometry(int argc, char **argv);
	static void test_card(int argc, char **argv);
};


