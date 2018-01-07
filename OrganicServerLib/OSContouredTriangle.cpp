#include "stdafx.h"
#include <iostream>
#include "OSContouredTriangle.h"

OSContouredTriangle::OSContouredTriangle(OSTrianglePoint in_point0, OSTrianglePoint in_point1, OSTrianglePoint in_point2)
{
	trianglePoints[0] = in_point0;
	trianglePoints[1] = in_point1;
	trianglePoints[2] = in_point2;

	//std::cout << "triangle points: -----------" << std::endl;
	//std::cout << "point 0: " << trianglePoints[0].x << ", " << trianglePoints[0].y << ", " << trianglePoints[0].z << std::endl;
	//std::cout << "point 1: " << trianglePoints[1].x << ", " << trianglePoints[1].y << ", " << trianglePoints[1].z << std::endl;
	//std::cout << "point 2: " << trianglePoints[2].x << ", " << trianglePoints[2].y << ", " << trianglePoints[2].z << std::endl;

}

void OSContouredTriangle::addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType)
{
	polygonPieceMap[in_Key] = in_pieceType;
}

OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}