#pragma once
#include "OSTrianglePoint.h"
#include "EnclaveKeyDef.h"
#include "OSTriangleLine.h"
#include "ECBBorderValues.h"
#include <unordered_map>
#include <math.h>

class OSContouredTriangle {
public:
	ECBPolyPoint trianglePoints[3];	// stores 3 points
	//OSTrianglePoint...
	OSTriangleLine	triangleLines[3];	// stores 3 lines

	OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2);
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> polygonPieceMap;			// stores the type of polygon piece, for each blueprint touched (intersected?) by this plan
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBBorderValues, EnclaveKeyDef::KeyHasher> ecbBorderMap;
	OSContouredTriangle();
	void addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType);
	void determineLineLengths();		// gets the length of lines
	void determineLineAngles();			// determines two angles for each line: an angle of rotation towards y axis, and an angle towards the 3rd point of the triangle
};
