#pragma once

#ifndef OSCONTOUREDTRIANGLE_H
#define OSCONTOUREDTRIANGLE_H

#include "OSTrianglePoint.h"
#include "EnclaveKeyDef.h"
#include "OSTriangleLine.h"
#include "ECBBorderValues.h"
#include "ECBPoly.h"
#include <unordered_map>
#include <math.h>

class OSContouredTriangle {
public:
	ECBPolyPoint trianglePoints[3];				// stores 3 points
	EnclaveKeyDef::EnclaveKey pointKeys[3];		// stores the keys of the 3 points
	OSTriangleLine	triangleLines[3];			// stores 3 lines

	OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2);
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> polygonPieceMap;			// stores the ID of the corresponding ECBPolygon(s) related to this instance of OSContouredTriangle, the ID of which is found in each blueprint.
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBBorderValues, EnclaveKeyDef::KeyHasher> ecbBorderMap;
	//std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> 
	OSContouredTriangle();
	int isTriangleFlat = 0;				// set to 1 if the Triangle is flat (all points on same y level)
	int perfect_clamp_x = 0;			// are all 3 points of this triangle perfectly on x axis?
	int perfect_clamp_y = 0;			// ...y axis?
	int perfect_clamp_z = 0;			// ...z axis?
	void addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType);
	void determineLineLengths();		// gets the length of lines
	void determineAxisInterceptDistances();
	void determineLineAxisIntercept(OSTriangleLine* in_LinePtr, ECBPolyPoint in_thirdPoint);
	void determineType2and3Lines();		// calculates the Type 2 and Type 3 lines for each line in the triangle
	void rotateTriangleFromZAxis(int in_Point);
	void determineLineAngles();			// determines two angles for each line: an angle of rotation towards y axis, and an angle towards the 3rd point of the triangle
};

#endif