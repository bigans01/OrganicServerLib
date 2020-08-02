#include "stdafx.h"
#include <iostream>
#include "OSContouredTriangle.h"
#include "OrganicUtils.h"
#include "ECBBorderValues.h"
#include "PolyUtils.cpp"
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

OSContouredTriangle::OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, int in_materialID, ECBPolyPoint in_massReferencePoint, ForgedPolyRegistry* in_forgedPolyRegistryRef)
{
	trianglePoints[0] = in_point0;
	trianglePoints[1] = in_point1;
	trianglePoints[2] = in_point2;
	materialID = in_materialID;
	massReferencePoint = in_massReferencePoint;
	forgedPolyRegistryRef = in_forgedPolyRegistryRef;

	glm::vec3 glmPoint0, glmPoint1, glmPoint2, glmMRP;
	glmPoint0.x = trianglePoints[0].x;
	glmPoint0.y = trianglePoints[0].y;
	glmPoint0.z = trianglePoints[0].z;

	glmPoint1.x = trianglePoints[1].x;
	glmPoint1.y = trianglePoints[1].y;
	glmPoint1.z = trianglePoints[1].z;

	glmPoint2.x = trianglePoints[2].x;
	glmPoint2.y = trianglePoints[2].y;
	glmPoint2.z = trianglePoints[2].z;

	glmMRP.x = massReferencePoint.x;
	glmMRP.y = massReferencePoint.y;
	glmMRP.z = massReferencePoint.z;

	EmptyNormalFinder normalFinder(glmPoint0, glmPoint1, glmPoint2, glmMRP);

	contouredEmptyNormal.x = normalFinder.calculatedNormal.x;
	contouredEmptyNormal.y = normalFinder.calculatedNormal.y;
	contouredEmptyNormal.z = normalFinder.calculatedNormal.z;

}

void OSContouredTriangle::determineLineLengths()
{
	// calc line 1, aka "0"
	//std::cout << "Determining line lengths, points..." << std::endl;
	TriangleLine* triangleLineRef_0 = &triangleLines[0];
	ECBPolyPoint* point_A_ref = &trianglePoints[0];
	ECBPolyPoint* point_B_ref = &trianglePoints[1];
	ECBPolyPoint* point_C_ref = &trianglePoints[2];
	
	//triangleLineRef_0->pointA.x = point_A_ref->x;
	//triangleLineRef_0->pointA.y = point_A_ref->y;
	//triangleLineRef_0->pointA.z = point_A_ref->z;
	//triangleLineRef_0->pointB.x = point_B_ref->x;
	//triangleLineRef_0->pointB.y = point_B_ref->y;
	//triangleLineRef_0->pointB.z = point_B_ref->z;

	triangleLineRef_0->pointA = *point_A_ref;
	triangleLineRef_0->pointB = *point_B_ref;
	triangleLineRef_0->pointC = *point_C_ref;


	float x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	float y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	float z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_0->lineLength = sqrt(x_pow + y_pow + z_pow);
	//std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;

	// calc line 2, aka "1"
	TriangleLine* triangleLineRef_1 = &triangleLines[1];
	point_A_ref = &trianglePoints[1];
	point_B_ref = &trianglePoints[2];
	point_C_ref = &trianglePoints[0];
	//triangleLineRef_1->pointA.x = point_A_ref->x;
	//triangleLineRef_1->pointA.y = point_A_ref->y;
	//triangleLineRef_1->pointA.z = point_A_ref->z;
	//triangleLineRef_1->pointB.x = point_B_ref->x;
	//triangleLineRef_1->pointB.y = point_B_ref->y;
	//triangleLineRef_1->pointB.z = point_B_ref->z;
	triangleLineRef_1->pointA = *point_A_ref;
	triangleLineRef_1->pointB = *point_B_ref;
	triangleLineRef_1->pointC = *point_C_ref;

	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_1->lineLength = sqrt(x_pow + y_pow + z_pow);
	//std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;

	// calc line 2, aka "3"
	TriangleLine* triangleLineRef_2 = &triangleLines[2];
	point_A_ref = &trianglePoints[2];
	point_B_ref = &trianglePoints[0];
	point_C_ref = &trianglePoints[1];
	//triangleLineRef_2->pointA.x = point_A_ref->x;
	//triangleLineRef_2->pointA.y = point_A_ref->y;
	//triangleLineRef_2->pointA.z = point_A_ref->z;
	//triangleLineRef_2->pointB.x = point_B_ref->x;
	//triangleLineRef_2->pointB.y = point_B_ref->y;
	//triangleLineRef_2->pointB.z = point_B_ref->z;
	triangleLineRef_2->pointA = *point_A_ref;
	triangleLineRef_2->pointB = *point_B_ref;
	triangleLineRef_2->pointC = *point_C_ref;

	x_pow = pow((point_B_ref->x - point_A_ref->x), 2.0f);	// pythagorean theorem calcs
	y_pow = pow((point_B_ref->y - point_A_ref->y), 2.0f);
	z_pow = pow((point_B_ref->z - point_A_ref->z), 2.0f);
	triangleLineRef_2->lineLength = sqrt(x_pow + y_pow + z_pow);
	//std::cout << point_B_ref->x << ", " << point_B_ref->y << ", " << point_B_ref->z << std::endl;
	//float point_B_ref->yx_pow = pow((triangleLineRef_1->pointB.x))
	//std::cout << "line lengths: " << std::endl;
	//std::cout << triangleLineRef_0->lineLength << std::endl;
	//std::cout << triangleLineRef_1->lineLength << std::endl;
	//std::cout << triangleLineRef_2->lineLength << std::endl;

	//std::cout << "line points::: " << std::endl; 
}

void OSContouredTriangle::addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType)
{
	//std::cout << "!! Adding polygon piece: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << std::endl;
	//polygonPieceMap[in_Key] = in_pieceType;
	polygonPieceMap.emplace(in_Key, in_pieceType);
	//someVector.push_back(in_pieceType);
	//polygonPieceMap[in_Key] = 0;
}

void OSContouredTriangle::determineLineAngles() 
{
	// determine Y angle for point 0 to point 1

	for (int x = 0; x < 3; x++)
	{


		TriangleLine* line0 = &triangleLines[x];
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

void OSContouredTriangle::insertTracedBlueprint(EnclaveKeyDef::EnclaveKey in_key)
{
	tracedBlueprintCountMap[in_key]++;	// increment the count
}

bool OSContouredTriangle::checkIfPointsAreInSameBlueprint()
{
	bool result = true; 
	if
		(
		(pointKeys[0].x == pointKeys[1].x)
			&&
			(pointKeys[1].x == pointKeys[2].x)

			&&

			(pointKeys[0].y == pointKeys[1].y)
			&&
			(pointKeys[1].y == pointKeys[2].y)

			&&

			(pointKeys[0].z == pointKeys[1].z)
			&&
			(pointKeys[1].z == pointKeys[2].z)
			)
	{
		//std::cout << "!!!! ContouredTriangle is isolated to one blueprint. " << std::endl;
	}
	else
	{
		//std::cout << "!!!! ContouredTriangle spans multiple blueprints...filling checks required. " << std::endl;
		result = false;
	}

	//std::cout << "Check if points in same blueprint call complete..." << std::endl;
	containedWithinSameBlueprint = result;
	return result;
}

void OSContouredTriangle::determineType2and3Lines()
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyIter = polygonPieceMap.begin();
	for (polyIter; polyIter != polygonPieceMap.end(); polyIter++)
	{
		EnclaveKeyDef::EnclaveKey currentBlueprintKey = polyIter->first;
		int currentBlueprintPoly = polyIter->second;
		ECBPoly* polyPtr;
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
	//determineLineAxisIntercept(&triangleLines[0], trianglePoints[2]);
	//determineLineAxisIntercept(&triangleLines[1], trianglePoints[0]);
	//determineLineAxisIntercept(&triangleLines[2], trianglePoints[1]);

	PolyUtils::determineLineInterceptSlopes(&triangleLines[0], trianglePoints[2]);
	//PolyUtils::sillyTest();
	PolyUtils::determineLineInterceptSlopes(&triangleLines[1], trianglePoints[0]);
	PolyUtils::determineLineInterceptSlopes(&triangleLines[2], trianglePoints[1]);
}

void OSContouredTriangle::determineAxisInterceptDistancesDebug()
{
	//determineLineAxisInterceptDebug(&triangleLines[0], trianglePoints[2], 0);
	//determineLineAxisInterceptDebug(&triangleLines[1], trianglePoints[0], 1);
	//determineLineAxisInterceptDebug(&triangleLines[2], trianglePoints[1], 2);

	PolyUtils::determineLineInterceptSlopesDebug(&triangleLines[0], trianglePoints[2], 0);
	PolyUtils::determineLineInterceptSlopesDebug(&triangleLines[1], trianglePoints[0], 1);
	PolyUtils::determineLineInterceptSlopesDebug(&triangleLines[2], trianglePoints[1], 2);
}

OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}

void OSContouredTriangle::addNewPrimarySegment(ECBPolyPoint in_lineSegmentPointA, ECBPolyPoint in_lineSegmentPointB, int in_lineID, EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	primarySegmentTrackerMap[in_blueprintKey].insertNewSegment(in_lineSegmentPointA, in_lineSegmentPointB, in_lineID);
}

void OSContouredTriangle::fillMetaDataInPrimaryCircuits()
{
	auto trackerMapBegin = primarySegmentTrackerMap.begin();
	auto trackerMapEnd = primarySegmentTrackerMap.end();
	for (trackerMapBegin; trackerMapBegin != trackerMapEnd; trackerMapBegin++)
	{
		trackerMapBegin->second.setBorderData(trackerMapBegin->first);
		trackerMapBegin->second.fillSegmentArrayMetaData(1);		// using 1 for testing; may be replaced later.
	}
}

void OSContouredTriangle::printPrimarySegmentData()
{
	auto trackerMapBegin = primarySegmentTrackerMap.begin();
	auto trackerMapEnd = primarySegmentTrackerMap.end();
	for (trackerMapBegin; trackerMapBegin != trackerMapEnd; trackerMapBegin++)
	{
		
		//std::cout << ":: Blueprint (" << trackerMapBegin->first.x << ", " << trackerMapBegin->first.y << ", " << trackerMapBegin->first.z << ") " << std::endl;
		for (int x = 0; x < trackerMapBegin->second.currentSegmentCount; x++)
		{
			//std::cout << "Segment " << x << ":-> Line ID: " << int(trackerMapBegin->second.primarySegments[x].lineID) << " | Point A: " << trackerMapBegin->second.primarySegments[x].beginPoint.x << ", " << trackerMapBegin->second.primarySegments[x].beginPoint.y << ", " << trackerMapBegin->second.primarySegments[x].beginPoint.z << " | Point B: " << trackerMapBegin->second.primarySegments[x].endPoint.x << ", " << trackerMapBegin->second.primarySegments[x].endPoint.y << ", " << trackerMapBegin->second.primarySegments[x].endPoint.z << std::endl;
		}
		
	}
}

void OSContouredTriangle::loadAndCalibrateKeyPairArray()
{
	// load the data
	keyPairArray[0].initialize(pointKeys[0], pointKeys[1], trianglePoints[0], trianglePoints[1], trianglePoints[2]);		// initialize first line: begin/end keys of points 0 and 1, respectively; point order 0 1 2
	keyPairArray[1].initialize(pointKeys[1], pointKeys[2], trianglePoints[1], trianglePoints[2], trianglePoints[0]);		// initialize second line: begin/end keys of points 1 and 2, respectively; point order 1 2 0
	keyPairArray[2].initialize(pointKeys[2], pointKeys[0], trianglePoints[2], trianglePoints[0], trianglePoints[1]);		// initialize second line: begin/end keys of points 1 and 2, respectively; point order 1 2 0

	// calibrate, but only if it isn't contained within the same blueprint.
	if (containedWithinSameBlueprint == false)
	{
		for (int x = 0; x < 3; x++)
		{
			std::cout << "############### Calibrating line: " << x << std::endl;
			keyPairArray[x].calibrate();
		}
	}
}

void OSContouredTriangle::printKeyPairArray()
{
	std::cout << "#### Printing key pair array: " << std::endl;
	for (int x = 0; x < 3; x++)
	{
		std::cout << "[" << x << "]: begin (" << keyPairArray[x].startKey.x << ", " << keyPairArray[x].startKey.y << ", " << keyPairArray[x].startKey.z << ") | end (" << keyPairArray[x].endKey.x << ", " << keyPairArray[x].endKey.y << ", " << keyPairArray[x].endKey.z << ") " << std::endl;
	}
}

