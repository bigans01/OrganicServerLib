#include "stdafx.h"
#include <iostream>
#include "OSContouredTriangle.h"
#include "OrganicUtils.h"
#include "ECBBorderValues.h"
#define PI 3.14159265f

OSContouredTriangle::OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2)
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
	ECBPolyPoint* point_A_ref = &trianglePoints[0];
	ECBPolyPoint* point_B_ref = &trianglePoints[1];
	
	//triangleLineRef_0->pointA.x = point_A_ref->x;
	//triangleLineRef_0->pointA.y = point_A_ref->y;
	//triangleLineRef_0->pointA.z = point_A_ref->z;
	//triangleLineRef_0->pointB.x = point_B_ref->x;
	//triangleLineRef_0->pointB.y = point_B_ref->y;
	//triangleLineRef_0->pointB.z = point_B_ref->z;

	triangleLineRef_0->pointA = *point_A_ref;
	triangleLineRef_0->pointB = *point_B_ref;


	float x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	float y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	float z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_0->lineLength = sqrt(x_pow + y_pow + z_pow);
	std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;

	// calc line 2, aka "1"
	OSTriangleLine* triangleLineRef_1 = &triangleLines[1];
	point_A_ref = &trianglePoints[1];
	point_B_ref = &trianglePoints[2];
	//triangleLineRef_1->pointA.x = point_A_ref->x;
	//triangleLineRef_1->pointA.y = point_A_ref->y;
	//triangleLineRef_1->pointA.z = point_A_ref->z;
	//triangleLineRef_1->pointB.x = point_B_ref->x;
	//triangleLineRef_1->pointB.y = point_B_ref->y;
	//triangleLineRef_1->pointB.z = point_B_ref->z;
	triangleLineRef_1->pointA = *point_A_ref;
	triangleLineRef_1->pointB = *point_B_ref;

	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_1->lineLength = sqrt(x_pow + y_pow + z_pow);
	std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;

	// calc line 2, aka "3"
	OSTriangleLine* triangleLineRef_2 = &triangleLines[2];
	point_A_ref = &trianglePoints[2];
	point_B_ref = &trianglePoints[0];
	//triangleLineRef_2->pointA.x = point_A_ref->x;
	//triangleLineRef_2->pointA.y = point_A_ref->y;
	//triangleLineRef_2->pointA.z = point_A_ref->z;
	//triangleLineRef_2->pointB.x = point_B_ref->x;
	//triangleLineRef_2->pointB.y = point_B_ref->y;
	//triangleLineRef_2->pointB.z = point_B_ref->z;
	triangleLineRef_2->pointA = *point_A_ref;
	triangleLineRef_2->pointB = *point_B_ref;

	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_2->lineLength = sqrt(x_pow + y_pow + z_pow);
	std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;
		//float point_B_ref->yx_pow = pow((triangleLineRef_1->pointB.x))
	std::cout << "line lengths: " << std::endl;
	std::cout << triangleLineRef_0->lineLength << std::endl;
	std::cout << triangleLineRef_1->lineLength << std::endl;
	std::cout << triangleLineRef_2->lineLength << std::endl;

	std::cout << "line points::: " << std::endl; 
}

void OSContouredTriangle::addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType)
{
	polygonPieceMap[in_Key] = in_pieceType;
}

void OSContouredTriangle::determineLineAngles() 
{
	// determine Y angle for point 0 to point 1

	for (int x = 0; x < 3; x++)
	{


		OSTriangleLine* line0 = &triangleLines[x];
		//std::cout << "line pointA.y: " << line0->pointA.x << ", " <<  line0->pointA.y << ", " <<  line0->pointA.z << std::endl;
		//std::cout << "line pointB.y: " << line0->pointB.x << ", " <<  line0->pointB.y << ", " <<  line0->pointB.z << std::endl;
		//std::cout << "line length: " << line0->lineLength << std::endl;



		if (line0->pointA.y == line0->pointB.y)		// both points are 0 (perfect contour line on y axis)
		{
			line0->angleToYaxis = 0.0f;	// set angle to 0
			std::cout << "cos test" << (acos(1.0f / 1.44f) * (180 / PI)) << std::endl;
		}
		else
		{
			if (line0->pointA.y > line0->pointB.y)		// elevation of A is greater than B
			{
				OSTrianglePoint newPoint, newPoint2;			// new point has same Y as A, but has B's x/z
				newPoint.x = line0->pointB.x;
				newPoint.y = line0->pointA.y;
				newPoint.z = line0->pointA.z;		// if viewing from Z-axis, this needs to be the same as point A.

				newPoint2.x = line0->pointB.x;
				newPoint2.y = line0->pointB.y;
				newPoint2.z = line0->pointA.z;

				float zViewAdjacentLine_Xpow = pow((newPoint.x - line0->pointA.x), 2.0f);
				float zViewAdjacentLine_Ypow = pow((newPoint.y - line0->pointA.y), 2.0f);
				float zViewAdjacentLine_Zpow = pow((newPoint.z - line0->pointA.z), 2.0f);
				float zViewAdjacentLine = sqrt(zViewAdjacentLine_Xpow + zViewAdjacentLine_Ypow + zViewAdjacentLine_Zpow);

				float zViewHypotenuseLine_Xpow = pow((newPoint2.x - line0->pointA.x), 2.0f);
				float zViewHypotenuseLine_Ypow = pow((newPoint2.y - line0->pointA.y), 2.0f);
				float zViewHypotenuseLine_Zpow = pow((newPoint2.z - line0->pointA.z), 2.0f);
				float zViewHypotenuseLine = sqrt(zViewHypotenuseLine_Xpow + zViewHypotenuseLine_Ypow + zViewHypotenuseLine_Zpow);



				// get distance between A and the new point
				//float x_pow = pow((newPoint.x - line0->pointA.x), 2.0f);
				//float y_pow = pow((newPoint.y - line0->pointA.y), 2.0f);
				//float z_pow = pow((newPoint.z - line0->pointA.z), 2.0f);
				//float newLineLength = sqrt(x_pow + y_pow + z_pow);

				//line0->angleToYaxis = acos(newLineLength / line0->lineLength) * (180 / PI);		// convert to degrees
				line0->angleToYaxis = acos(zViewAdjacentLine / zViewHypotenuseLine) * (180 / PI);

				std::cout << "A is greater, Angle is: " << line0->angleToYaxis << std::endl;
				std::cout << "Performing rotation on z-axis..." << std::endl;
				//rotateTriangleFromZAxis(x);
			}
			else if (line0->pointA.y < line0->pointB.y)	// elevation of A is less than B
			{
				OSTrianglePoint newPoint, newPoint2;			// new point has same Y as B, but has A's x/z
				newPoint.x = line0->pointA.x;
				newPoint.y = line0->pointB.y;
				newPoint.z = line0->pointB.z;

				newPoint2.x = line0->pointA.x;
				newPoint2.y = line0->pointA.y;
				newPoint2.z = line0->pointB.z;

				float zViewAdjacentLine_Xpow = pow((newPoint.x - line0->pointB.x), 2.0f);
				float zViewAdjacentLine_Ypow = pow((newPoint.y - line0->pointB.y), 2.0f);
				float zViewAdjacentLine_Zpow = pow((newPoint.z - line0->pointB.z), 2.0f);
				float zViewAdjacentLine = sqrt(zViewAdjacentLine_Xpow + zViewAdjacentLine_Ypow + zViewAdjacentLine_Zpow);

				float zViewHypotenuseLine_Xpow = pow((newPoint2.x - line0->pointB.x), 2.0f);
				float zViewHypotenuseLine_Ypow = pow((newPoint2.y - line0->pointB.y), 2.0f);
				float zViewHypotenuseLine_Zpow = pow((newPoint2.z - line0->pointB.z), 2.0f);
				float zViewHypotenuseLine = sqrt(zViewHypotenuseLine_Xpow + zViewHypotenuseLine_Ypow + zViewHypotenuseLine_Zpow);

				// get distance between A and the new point
				//float x_pow = pow((newPoint.x - line0->pointB.x), 2.0f);
				//float y_pow = pow((newPoint.y - line0->pointB.y), 2.0f);
				//float z_pow = pow((newPoint.z - line0->pointB.z), 2.0f);
				//float newLineLength = sqrt(x_pow + y_pow + z_pow);
				std::cout << "adjacent length: " << zViewAdjacentLine << std::endl;
				std::cout << "hypotenuse length: " << zViewHypotenuseLine << std::endl;

				line0->angleToYaxis = acos(zViewAdjacentLine / zViewHypotenuseLine) * (180 / PI);
				std::cout << "B is greater, Angle is: " << line0->angleToYaxis << std::endl;
				std::cout << "Performing rotation on z-axis..." << std::endl;
				//rotateTriangleFromZAxis(x);
			}
		}

	}
}

void OSContouredTriangle::rotateTriangleFromZAxis(int in_Point)
{
	if (in_Point == 0) // first point, index 0 (line is also 0)
	{
		ECBPolyPoint pointToShiftTo0 = trianglePoints[0];
		ECBPolyPoint point2 = trianglePoints[1];
		ECBPolyPoint point3 = trianglePoints[2];
		float xShiftValue = pointToShiftTo0.x;		// get shifting values from pointToShift0
		float yShiftValue = pointToShiftTo0.y;				
		pointToShiftTo0.x -= pointToShiftTo0.x;		// set the x to 0.0f;
		pointToShiftTo0.y -= pointToShiftTo0.y;
		point2.x -= pointToShiftTo0.x;		// do the same for the 2nd point
		point2.y -= pointToShiftTo0.y;
		point3.x -= pointToShiftTo0.x;		// "" "" the 3rd point
		point3.y -= pointToShiftTo0.y;

		float rotationAngle = triangleLines[0].angleToYaxis;	// grab the y axis rotation angle (viewed from z)
		float radianAngle = OrganicUtils::degreesToPiRads(rotationAngle);
		// begin rotation calculations
		point2.x = ((cos(radianAngle)*point2.x) + (sin(radianAngle)*point2.y));
		point2.y = (-(sin(radianAngle)*point2.x) + (cos(radianAngle)*point2.y));

		point3.x = ((cos(radianAngle)*point3.x) + (sin(radianAngle)*point3.y));
		point3.y = (-(sin(radianAngle)*point3.x) + (cos(radianAngle)*point3.y));

	}
	else if (in_Point == 1) // second point, index 1
	{
		ECBPolyPoint pointToShiftTo0 = trianglePoints[1];
		ECBPolyPoint point2 = trianglePoints[2];
		ECBPolyPoint point3 = trianglePoints[0];
		float xShiftValue = pointToShiftTo0.x;		// get shifting values from pointToShift0
		float yShiftValue = pointToShiftTo0.y;
		pointToShiftTo0.x -= pointToShiftTo0.x;		// set the x to 0.0f;
		pointToShiftTo0.y -= pointToShiftTo0.y;
		point2.x -= pointToShiftTo0.x;		// do the same for the 2nd point
		point2.y -= pointToShiftTo0.y;
		point3.x -= pointToShiftTo0.x;		// "" "" the 3rd point
		point3.y -= pointToShiftTo0.y;

		float rotationAngle = triangleLines[1].angleToYaxis;	// grab the y axis rotation angle (viewed from z)
		float radianAngle = OrganicUtils::degreesToPiRads(rotationAngle);
		// begin rotation calculations
		point2.x = ((cos(radianAngle)*point2.x) + (sin(radianAngle)*point2.y));
		point2.y = (-(sin(radianAngle)*point2.x) + (cos(radianAngle)*point2.y));

		point3.x = ((cos(radianAngle)*point3.x) + (sin(radianAngle)*point3.y));
		point3.y = (-(sin(radianAngle)*point3.x) + (cos(radianAngle)*point3.y));
	}
	else if (in_Point == 2) // third point, index 2
	{
		ECBPolyPoint pointToShiftTo0 = trianglePoints[2];
		ECBPolyPoint point2 = trianglePoints[1];
		ECBPolyPoint point3 = trianglePoints[0];
		float xShiftValue = pointToShiftTo0.x;		// get shifting values from pointToShift0
		float yShiftValue = pointToShiftTo0.y;
		pointToShiftTo0.x -= pointToShiftTo0.x;		// set the x to 0.0f;
		pointToShiftTo0.y -= pointToShiftTo0.y;
		point2.x -= pointToShiftTo0.x;		// do the same for the 2nd point
		point2.y -= pointToShiftTo0.y;
		point3.x -= pointToShiftTo0.x;		// "" "" the 3rd point
		point3.y -= pointToShiftTo0.y;

		float rotationAngle = triangleLines[2].angleToYaxis;	// grab the y axis rotation angle (viewed from z)
		float radianAngle = OrganicUtils::degreesToPiRads(rotationAngle);
		// begin rotation calculations
		point2.x = ((cos(radianAngle)*point2.x) + (sin(radianAngle)*point2.y));
		point2.y = (-(sin(radianAngle)*point2.x) + (cos(radianAngle)*point2.y));

		point3.x = ((cos(radianAngle)*point3.x) + (sin(radianAngle)*point3.y));
		point3.y = (-(sin(radianAngle)*point3.x) + (cos(radianAngle)*point3.y));
	}
}

void OSContouredTriangle::determineAxisInterceptDistances()
{
	determineLineAxisIntercept(&triangleLines[0], trianglePoints[2]);
	determineLineAxisIntercept(&triangleLines[1], trianglePoints[0]);
	determineLineAxisIntercept(&triangleLines[2], trianglePoints[1]);
}

void OSContouredTriangle::determineLineAxisIntercept(OSTriangleLine* in_LinePtr, ECBPolyPoint in_thirdPoint)
{
	// X axis	||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	float line_pointA_x = in_LinePtr->pointA.x;		// get the x values of each point
	float line_pointB_x = in_LinePtr->pointB.x;
	float thirdPoint_x = in_thirdPoint.x;



	ECBPolyPoint currentSlope;						// get the current slope (this may be switched later)
	currentSlope.x = in_LinePtr->pointB.x - in_LinePtr->pointA.x;
	currentSlope.y = in_LinePtr->pointB.y - in_LinePtr->pointA.y;
	currentSlope.z = in_LinePtr->pointB.z - in_LinePtr->pointA.z;

	ECBPolyPoint currentSlopeNormalized;
	currentSlopeNormalized.x = (in_LinePtr->pointB.x - in_LinePtr->pointA.x) / abs(in_LinePtr->pointB.x - in_LinePtr->pointA.x);
	currentSlopeNormalized.y = (in_LinePtr->pointB.y - in_LinePtr->pointA.y) / abs(in_LinePtr->pointB.y - in_LinePtr->pointA.y);
	currentSlopeNormalized.z = (in_LinePtr->pointB.z - in_LinePtr->pointA.z) / abs(in_LinePtr->pointB.z - in_LinePtr->pointA.z);

	//std::cout << "-------------float test----------" << std::endl;
	//std::cout << "Value 1: " << -83.0f / -16.0645f << std::endl;
	//std::cout << "Value 2: " << 16.0833f / 3.1129f << std::endl;

	// when slope is positive...
	if ((thirdPoint_x > line_pointA_x) && (thirdPoint_x < line_pointB_x))	// the third point's x lies between the line's point A and B
	{
		std::cout << "X is between A and B points..." << std::endl;
		float newPointTravelDistance = (thirdPoint_x - line_pointA_x);		// distance on the line that the new point will have to travel
		float newPointSlope = newPointTravelDistance / currentSlope.x;		// get the new slope calculation for this point's x (y, z?)
		ECBPolyPoint newPoint;												// value that stores the new point coordinates
		newPoint.x = thirdPoint_x;
		newPoint.y = in_LinePtr->pointA.y + (currentSlope.y * newPointSlope);
		newPoint.z = in_LinePtr->pointA.z + (currentSlope.z * newPointSlope);

		std::cout << "current normalized slope: " << currentSlopeNormalized.x << ", " << currentSlopeNormalized.y << ", " << currentSlopeNormalized.z << std::endl;
		std::cout << "slope multiply test: " << currentSlope.y * newPointSlope << std::endl;
		std::cout << "----First point coordinates----: " << in_LinePtr->pointA.x << ",  " << in_LinePtr->pointA.y << ", " << in_LinePtr->pointA.z << std::endl;
		std::cout << "----Second point coordinates----: " << in_LinePtr->pointB.x << ",  " << in_LinePtr->pointB.y << ", " << in_LinePtr->pointB.z << std::endl;
		std::cout << "----Third point coordinates----: " << in_thirdPoint.x << ", " << in_thirdPoint.y << ", " << in_thirdPoint.z << std::endl;
		std::cout << "----New point coordinates----  : " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << std::endl;

		ECBPolyPoint temp_x_interceptSlope = OrganicUtils::findSlope(newPoint, in_thirdPoint);
		//temp_x_interceptSlope.x = in_thirdPoint.x - newPoint.x;
		//temp_x_interceptSlope.y = in_thirdPoint.y - newPoint.y;
		//temp_x_interceptSlope.z = in_thirdPoint.z - newPoint.z;
		std::cout << "X-intercept slope is: " << temp_x_interceptSlope.x << ", " << temp_x_interceptSlope.y << ", " << temp_x_interceptSlope.z << std::endl;
		in_LinePtr->x_interceptSlope = temp_x_interceptSlope;
	}
	else if 
			(
				((thirdPoint_x > line_pointB_x) && currentSlopeNormalized.x == 1)		// slope is positive; third point's x would have to be greater than point B's x
				||
				((thirdPoint_x < line_pointB_x) && currentSlopeNormalized.x == -1)		// slope is negative; third point's x would have to be less than point B's x
			)	// thirdPoint's x value is further past point B 
				{
					if (currentSlopeNormalized.x == 1)
					{
						std::cout << "X slope is positive, but third point's X is greater than point B's X" << std::endl;
						float line_length_to_new_point = abs(in_thirdPoint.x - in_LinePtr->pointA.x);
						float original_line_length = abs(in_LinePtr->pointB.x - in_LinePtr->pointA.x);	// distance from point A to point B (x)
						float slope_multiplier = line_length_to_new_point / original_line_length;
						ECBPolyPoint newPoint;
						newPoint.x = thirdPoint_x;
						newPoint.y = in_LinePtr->pointA.y + (currentSlope.y * slope_multiplier);
						newPoint.z = in_LinePtr->pointA.z + (currentSlope.z * slope_multiplier);
						std::cout << "New point is: " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << std::endl;
						ECBPolyPoint temp_x_interceptSlope = OrganicUtils::findSlope(newPoint, in_thirdPoint);
						std::cout << "X-intercept slope is: " << temp_x_interceptSlope.x << ", " << temp_x_interceptSlope.y << ", " << temp_x_interceptSlope.z << std::endl;
						in_LinePtr->x_interceptSlope = temp_x_interceptSlope;		// store the result in the line's x-intercept slope value

					}
					else if (currentSlopeNormalized.x == -1)
					{
						std::cout << "X slope is negative, but third point's X is less than point B's x" << std::endl;
						float line_length_to_new_point = abs(in_thirdPoint.x - in_LinePtr->pointA.x);	// distance from point A to the new point (x)
						float original_line_length = abs(in_LinePtr->pointB.x - in_LinePtr->pointA.x);	// distance from point A to point B (x)
						float slope_multiplier = line_length_to_new_point / original_line_length;
						ECBPolyPoint newPoint;
						newPoint.x = thirdPoint_x;
						newPoint.y = in_LinePtr->pointA.y + (currentSlope.y * slope_multiplier);
						newPoint.z = in_LinePtr->pointA.z + (currentSlope.z * slope_multiplier);
						std::cout << "New point is: " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << std::endl;
						ECBPolyPoint temp_x_interceptSlope = OrganicUtils::findSlope(newPoint, in_thirdPoint);
						std::cout << "X-intercept slope is: " << temp_x_interceptSlope.x << ", " << temp_x_interceptSlope.y << ", " << temp_x_interceptSlope.z << std::endl;
						in_LinePtr->x_interceptSlope = temp_x_interceptSlope;		// store the result in the line's x-intercept slope value
					}
				}
	else if 
			(
				((thirdPoint_x < line_pointA_x) && currentSlopeNormalized.x == 1)		// slope is positive, but third point's x would have to be less than point A's x
				||
				((thirdPoint_x > line_pointA_x) && currentSlopeNormalized.x == -1)		// slope is negative, but third point's x would have to be greater than point A's x
			)	
			
				{
					if (currentSlopeNormalized.x == 1)
					{
						std::cout << "X slope is positive, but third point's X is less than than point A's X" << std::endl;
						currentSlope = OrganicUtils::findSlope(in_LinePtr->pointB, in_LinePtr->pointA);	// reverse the slope
						float line_length_to_new_point = abs(in_thirdPoint.x - in_LinePtr->pointB.x);	// needs to be reversed; distance is to new point now comes as the distance between point B and the new point's x
						float original_line_length = abs(in_LinePtr->pointB.x - in_LinePtr->pointA.x);	// this doesn't need to be reversed since it's always an absolute value
						float slope_multiplier = line_length_to_new_point / original_line_length;
						ECBPolyPoint newPoint;
						newPoint.x = thirdPoint_x;
						newPoint.y = in_LinePtr->pointB.y + (currentSlope.y * slope_multiplier);
						newPoint.z = in_LinePtr->pointB.z + (currentSlope.z * slope_multiplier);
						std::cout << "New point is: " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << std::endl;
						ECBPolyPoint temp_x_interceptSlope = OrganicUtils::findSlope(newPoint, in_thirdPoint);
						std::cout << "X-intercept slope is: " << temp_x_interceptSlope.x << ", " << temp_x_interceptSlope.y << ", " << temp_x_interceptSlope.z << std::endl;
						in_LinePtr->x_interceptSlope = temp_x_interceptSlope;		// store the result in the line's x-intercept slope value

					}
					else if (currentSlopeNormalized.x == -1)
					{
						std::cout << "X slope is negative, but third point's X is greater than point A's x" << std::endl;
						currentSlope = OrganicUtils::findSlope(in_LinePtr->pointB, in_LinePtr->pointA);	// reverse the slope
						float line_length_to_new_point = abs(in_thirdPoint.x - in_LinePtr->pointB.x);	// needs to be reversed; distance is to new point now comes as the distance between point B and the new point's x
						float original_line_length = abs(in_LinePtr->pointB.x - in_LinePtr->pointA.x);	// this doesn't need to be reversed since it's always an absolute value
						float slope_multiplier = line_length_to_new_point / original_line_length;
						ECBPolyPoint newPoint;
						newPoint.x = thirdPoint_x;
						newPoint.y = in_LinePtr->pointB.y + (currentSlope.y * slope_multiplier);
						newPoint.z = in_LinePtr->pointB.z + (currentSlope.z * slope_multiplier);
						std::cout << "New point is: " << newPoint.x << ", " << newPoint.y << ", " << newPoint.z << std::endl;
						ECBPolyPoint temp_x_interceptSlope = OrganicUtils::findSlope(newPoint, in_thirdPoint);
						std::cout << "X-intercept slope is: " << temp_x_interceptSlope.x << ", " << temp_x_interceptSlope.y << ", " << temp_x_interceptSlope.z << std::endl;
						in_LinePtr->x_interceptSlope = temp_x_interceptSlope;		// store the result in the line's x-intercept slope value
					}
				}

	// when slope is negative...


}

OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}

