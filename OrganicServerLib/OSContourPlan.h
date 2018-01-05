#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include "OSContourLine.h"
#include "OSContouredTriangleStrip.h"
#include "OSTrianglePoint.h"

//class OSServer;
class OSContourPlan
{
public:
	
	OSContourPlan(float in_x, float in_y, float in_z);
	OSContourPlan(OSTrianglePoint in_Point);
	OSContourPlan();

	OSContourPlan(const OSContourPlan& plan_a);				// explicit copy constructor
	OSContourPlan& operator=(OSContourPlan&& plan_a)
	{
		return *this;
	}

	OSContourPlan& operator=(const OSContourPlan& plan_a)
	{
		startPoint = plan_a.startPoint;
		return *this;
	}

	

	void addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints);	// adds a contour line, with the map ID of the line equal to contourLineCount
	OSContourLine* getContourLine(int in_lineId);
	void amplifyContourLinePoints(int in_lineID);
	void buildTriangleStrips(int layerDepth);
	void createFirstLayerTriangles();

private:
	friend class OSServer;
	unordered_map<int, OSContourLine> contourLineMap;
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;

	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	int planMode = 0;					// 0 is default
	int numberOfTriangleStrips = 0;
	OSTrianglePoint startPoint;			// the center x/y/z coord of the plan

};
