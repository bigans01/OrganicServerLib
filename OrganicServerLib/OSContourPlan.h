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
#include "OSTerrainFormation.h"
#include "OSTerrainFormation.h"
#include "ECBPolyPoint.h"
#include "OrganicUtils.h"
#include <mutex>
#include "CursorPathTraceContainer.h"

class OSContourPlan
{
public:
	
	OSContourPlan(OSPDir in_Dir, float in_x, float in_y, float in_z);
	OSContourPlan(OSTrianglePoint in_Point);
	OSContourPlan(ECBPolyPoint in_Point);
	OSContourPlan(OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	OSContourPlan();

	OSContourPlan(const OSContourPlan& plan_a);				// explicit copy constructor
	OSContourPlan& operator=(OSContourPlan&& plan_a)
	{
		planDirections = plan_a.planDirections;
		terrainFormation = plan_a.terrainFormation;
		/*
		unordered_map<int, OSContourLine>::iterator contourLineBegin = plan_a.contourLineMap.begin();
		unordered_map<int, OSContourLine>::iterator contourLineEnd = plan_a.contourLineMap.end();
		for (contourLineBegin; contourLineBegin != contourLineEnd; contourLineBegin++)
		{
			int currentIndex = contourLineBegin->first;
			contourLineMap[currentIndex] = contourLineBegin->second;
			std::cout << "Wooooo 1" << std::endl;
		}
		*/
		return *this;
	}

	OSContourPlan& operator=(const OSContourPlan& plan_a)
	{
		planDirections = plan_a.planDirections;
		terrainFormation = plan_a.terrainFormation;
		startPoint = plan_a.startPoint;
		std::cout << "Wooooo 2" << std::endl;
		return *this;
	}

	

	void addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint);	// adds a contour line, with the map ID of the line equal to contourLineCount
	OSContourLine* getContourLine(int in_lineId);

	void amplifyContourLinePoints(int in_lineID);
	void buildTriangleStrips(int layerDepth);
	void createFirstLayerTriangles();				// creates a triangle strip with the very first contour line, where the center of this strip (that is, the 3rd point) is equal to the "peak" of this plan
	void constructStripTriangles(int in_stripID, int in_materialID);
	void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, int in_triangleStripID, short in_materialID);
	void performSingleTriangleTest();
	void setFormationBaseContourPoints(OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void amplifyAllContourLinePoints();
	ECBPolyPoint startPoint;						// the center x/y/z coord of the plan
	OSContourPlanDirections planDirections;			// the default plan directions, in case there are any perfect clampings
	OSTerrainFormation terrainFormation = OSTerrainFormation::NOVAL;
private:
	friend class OSServer;
	unordered_map<int, OSContourLine> contourLineMap;
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;
	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	int planMode = 0;					// 0 is default
	int numberOfTriangleStrips = 0;

	void constructOuterQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID);
	void constructInnerQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID);
	//float layerDistance = 0.0f;
	//float startRadius = 0.0f;
	//float expansionValue
	

};

#endif