#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include "OSContourLine.h"
#include "OSContouredTriangleStrip.h"
#include "OSTrianglePoint.h"

class OSContourPlan
{
public:
	unordered_map<int, OSContourLine> contourLineMap;
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;
	//unordered_map<int, shitTest> shitTestMap;
	//vector<shitTest> shitty;
	//shitTest poopy;

	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	int planMode = 0;					// 0 is default
	OSTrianglePoint startPoint;			// the center x/y/z coord of the plan

	
	OSContourPlan(float in_x, float in_y, float in_z);
	OSContourPlan(OSTrianglePoint in_Point);
	OSContourPlan();
	

	void addContourLine(float in_baseDistance, float in_contourElevation, int in_numberOfPoints);	// adds a contour line, with the map ID of the line equal to contourLineCount
	void amplifyContourLinePoints(int in_lineID);
	void buildTriangleStrips(int layerDepth);
	void createFirstLayerTriangles();
};
