#pragma once
#include "OSTrianglePoint.h"
#include "EnclaveKeyDef.h"
#include "OSTriangleLine.h"
#include <unordered_map>

class OSContouredTriangle {
public:
	OSTrianglePoint trianglePoints[3];	// stores 3 points
	//OSTrianglePoint...
	OSTriangleLine	triangleLines[3];	// stores 3 lines

	OSContouredTriangle(OSTrianglePoint in_point0, OSTrianglePoint in_point1, OSTrianglePoint in_point2);
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> polygonPieceMap;			// stores the type of polygon piece, for each blueprint touched (intersected?) by this plan
	OSContouredTriangle();
	void addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType);
	void determineLineLengths();
};
