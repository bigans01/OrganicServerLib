#pragma once
#ifndef SHITTESTHOLDER_H
#define SHITTESTHOLDER_H
#include "stdafx.h"
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include "OSContourLine.h"
#include "OSContouredTriangleStrip.h"
#include "OSTrianglePoint.h"
#include <functional>
#include "shitTest.h"

class OSServer;
class shitTestHolder
{
public:
	friend class OSServer;

	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	int planMode = 0;					// 0 is default
	OSTrianglePoint startPoint;			// the center x/y/z coord of the plan

	shitTestHolder();
	shitTestHolder(int x);		

	unordered_map<int, shitTest> shitTestMap;
};

#endif