#pragma once

#ifndef CONTOUREDMOUNTAIN_H
#define CONTOUREDMOUNTAIN_H

#include "ContourBase.h"
#include "ECBPolyPoint.h"
#include "OSTerrainFormation.h"
#include "OSContourLine.h"
#include "CursorPathTraceContainer.h"

class ContouredMountain : public ContourBase
{
public:

	// definitions for virtual functions
	void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void amplifyAllContourLinePoints();
	void constructStripTriangles(int in_stripID, int in_materialID);
	void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID);

private:
	unordered_map<int, OSContourLine> contourLineMap;
	float currentRadius = 0;
	float currentNumberOfPoints = 0;
	int contourLineCount = 0;			// the number of contour lines in contourLineMap
	ECBPolyPoint startPoint;			// startPoint for the contoured mountain

	// class specific functions
	void setFormationBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint);	// adds a contour line, with the map ID of the line equal to contourLineCount
	void constructOuterQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID);
	void constructInnerQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID);
	void amplifyContourLinePoints(int in_lineID);

};

#endif
