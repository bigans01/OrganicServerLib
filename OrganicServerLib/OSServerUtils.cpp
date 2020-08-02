#include "stdafx.h"
#include "OSServerUtils.h"

void OSServerUtils::fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle)
{
	//std::cout << "#######!#############!######### Attempting clamp fill! " << std::endl;
	/*
	if (in_contouredTriangle->perfect_clamp_x == 1
		||
		in_contouredTriangle->perfect_clamp_y == 1
		||
		in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 1;
		std::cout << "Perfect clamp detected! " << std::endl;
	}
	*/
	if (in_contouredTriangle->perfect_clamp_x == 1)
	{
		//std::cout << "Perfectly clamped to X!" << std::endl;
		in_polyPtr->isPolyPerfectlyClamped = 1;
	}
	else if (in_contouredTriangle->perfect_clamp_y == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 2;
	}
	else if (in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 3;
	}
}
void OSServerUtils::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID)
{
	//in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_Triangle->triangleLines[in_pointID].pointA;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_Triangle->triangleLines[in_pointID].pointB;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z
}
void OSServerUtils::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint)
{
	//in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_beginPoint;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_endPoint;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z

	if
	(
		(in_LinePtr->pointA.y == 0)
		||
		(in_LinePtr->pointB.y == 0)
		||
		(in_LinePtr->pointC.y == 0)
	)
	{
		//std::cout << "+++++++++SPECIAL HALT " << std::endl;
		//int someVal = 3;
		//std::cin >> someVal;
	}
}