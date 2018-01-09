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

void OSContouredTriangle::determineLineLengths()
{
	// calc line 1, aka "0"
	OSTriangleLine* triangleLineRef_0 = &triangleLines[0];
	OSTrianglePoint* point_A_ref = &trianglePoints[0];
	OSTrianglePoint* point_B_ref = &trianglePoints[1];
	float x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	float y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	float z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_0->lineLength = sqrt(x_pow + y_pow + z_pow);

	// calc line 2, aka "1"
	OSTriangleLine* triangleLineRef_1 = &triangleLines[1];
	point_A_ref = &trianglePoints[1];
	point_B_ref = &trianglePoints[2];
	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_1->lineLength = sqrt(x_pow + y_pow + z_pow);

	// calc line 2, aka "3"
	OSTriangleLine* triangleLineRef_2 = &triangleLines[2];
	point_A_ref = &trianglePoints[2];
	point_B_ref = &trianglePoints[0];
	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_2->lineLength = sqrt(x_pow + y_pow + z_pow);

	//float x_pow = pow((triangleLineRef_1->pointB.x))
	std::cout << "line lengths: " << std::endl;
	std::cout << triangleLineRef_0->lineLength << std::endl;
	std::cout << triangleLineRef_1->lineLength << std::endl;
	std::cout << triangleLineRef_2->lineLength << std::endl;
}

void OSContouredTriangle::addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType)
{
	polygonPieceMap[in_Key] = in_pieceType;
}

OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}