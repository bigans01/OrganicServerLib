#pragma once

#ifndef CONTOUREDMOUNTAIN_H
#define CONTOUREDMOUNTAIN_H

#include "ContourBase.h"
#include "ECBPolyPoint.h"
#include "OSTerrainFormation.h"
#include "OSContourLine.h"
#include "CursorPathTraceContainer.h"
#include <map>

class ContouredMountain : public ContourBase
{
public:

	// definitions for virtual functions
	void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void amplifyAllContourLinePoints();
	void constructStripTriangles(int in_stripID, int in_materialID);
	void constructBottomStripTriangles(int in_stripID, int in_materialID);
	void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_type);		// for specifying triangles that are not FREE
	void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID);
	void runMassDrivers();

private:
	map<int, OSContourLine> contourLineMap;
	map<int, OSContourLine> bottomContourLineMap;

	map<int, ECBPolyPoint> triangleStripMRPMap;
	map<int, ECBPolyPoint> triangleBottomStripMRPMap;

	float currentRadius = 0;
	float currentNumberOfPoints = 0;
	int topContourLineCount = 0;			// the number of contour lines in the top contourLineMap
	int bottomContourLineCount = 0;			// the number of contour lines in the bottom contourLineMap
	ECBPolyPoint startPoint;			// startPoint for the top layer of the contoured mountain
	ECBPolyPoint bottomStartPoint;      // startPoint for the bottom layer of the contoured mountain
	int numberOfLayers = 0;				// the number of layers in the mountain.

	void setMRPsForBottomLayers();
	void constructStripTrianglesForLayer(map<int, OSContourLine>* in_contourLineMapRef, map<int, ECBPolyPoint>* in_triangleStripMRPMapRef, int in_stripID, int in_materialID, unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, ECBPolyType in_type);

	// class specific functions
	void setFormationBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void setFormationBottomBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	void addContourLine(map<int, OSContourLine>* in_contourLineMapRef, int* line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint);	// adds a contour line, with the map ID of the line equal to contourLineCount
	void constructOuterQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type);
	void constructInnerQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type);
	void amplifyContourLinePoints(int in_lineID);
	void insertMRP(map<int, ECBPolyPoint>* in_mrpMapRef, int* in_stripValue, ECBPolyPoint in_polyPoint);
	void contouredMountainConstructSingleContouredTriangle(unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_type);
};

#endif
