#pragma once
#ifndef SHITTEST_H
#define SHITTEST_H

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <mutex>



using namespace std;
class OSContourPlan;
class OSServer;
class shitTestHolder;
class shitTest
{
public:
	friend class OSServer;
	friend class shitTestHolder;
	shitTest::~shitTest();
	shitTest(int x);
	shitTest();

	
	shitTest& operator=(shitTest&& line_a)
	{
		//smartContourPoint = std::move(line_a.smartContourPoint);
		//numberOfPoints = line_a.numberOfPoints;
		whoaptr = std::move(line_a.whoaptr);
		//value = line_a.value;
		return *this;
	}
	
	
	shitTest& operator=(const shitTest& line_a)
	{
		//whoaptr = std::move(line_a.whoaptr);
		//value = line_a.value;
		return *this;
	}
	

	//int value = 0;
	unique_ptr<int> whoaptr;
};

#endif