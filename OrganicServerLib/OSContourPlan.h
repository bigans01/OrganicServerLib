#pragma once

#ifndef OSCONTOURPLAN_H
#define OSCONTOURPLAN_H

#include "stdafx.h"
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include "OSContourLine.h"
#include "OSPDir.h"
#include "OSContouredTriangleStrip.h"
#include "OSContourPlanDirections.h"
#include "OSTrianglePoint.h"
#include "ECBPolyPoint.h"
#include "OrganicUtils.h"
#include <mutex>
#include "CursorPathTraceContainer.h"

class OSContourPlan
{
public:
	
	OSContourPlan(OSPDir in_Dir, float in_x, float in_y, float in_z);
	OSContourPlan(OSTrianglePoint in_Point);
	OSContourPlan();

	OSContourPlan(const OSContourPlan& plan_a);				// explicit copy constructor
	OSContourPlan& operator=(OSContourPlan&& plan_a)
	{
		planDirections = plan_a.planDirections;
		return *this;
	}

	OSContourPlan& operator=(const OSContourPlan& plan_a)
	{
		planDirections = plan_a.planDirections;
		startPoint = plan_a.startPoint;
		return *this;
	}

	

	void addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint);	// adds a contour line, with the map ID of the line equal to contourLineCount
	OSContourLine* getContourLine(int in_lineId);

	void amplifyContourLinePoints(int in_lineID);
	void buildTriangleStrips(int layerDepth);
	void createFirstLayerTriangles();				// creates a triangle strip with the very first contour line, where the center of this strip (that is, the 3rd point) is equal to the "peak" of this plan
	void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, mutex& heapmutex);
	void performSingleTriangleTest();
	ECBPolyPoint startPoint;						// the center x/y/z coord of the plan
	OSContourPlanDirections planDirections;			// the default plan directions, in case there are any perfect clampings

private:
	friend class OSServer;
	unordered_map<int, OSContourLine> contourLineMap;
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;

	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	int planMode = 0;					// 0 is default
	int numberOfTriangleStrips = 0;
	

};

#endif