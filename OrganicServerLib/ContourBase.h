#pragma once

#ifndef CONTOURBASE_H
#define CONTOURBASE_H

#include "ECBPolyPoint.h"
#include "OSContouredTriangleStrip.h"
#include "OSContourPlanDirections.h"
#include <map>


// base class for all future contour plans (will eventually replace old contour plan)
class ContourBase
{
public:

	// member variables each derived class should have
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;
	OSContourPlanDirections planDirections;

	// virtual functions
	virtual void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue) = 0;
	virtual void amplifyAllContourLinePoints() = 0;
	virtual void constructStripTriangles(int in_stripID, int in_materialID) = 0;
	virtual void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID) = 0;
};

#endif
