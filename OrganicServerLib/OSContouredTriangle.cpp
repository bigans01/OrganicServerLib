#include "stdafx.h"
#include <iostream>
#include "OSContouredTriangle.h"
#include "OrganicUtils.h"
#include "ECBBorderValues.h"
#include "PolyUtils.cpp"
#define PI 3.14159265f


OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}

OSContouredTriangle::OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, int in_materialID, ECBPolyPoint in_massReferencePoint, ForgedPolyRegistry* in_forgedPolyRegistryRef, ECBPolyType in_polyType)
{
	contouredPolyType = in_polyType;

	trianglePoints[0] = in_point0;
	trianglePoints[1] = in_point1;
	trianglePoints[2] = in_point2;

	TriangleLine line0(trianglePoints[0], trianglePoints[1], trianglePoints[2]);
	TriangleLine line1(trianglePoints[1], trianglePoints[2], trianglePoints[0]);
	TriangleLine line2(trianglePoints[2], trianglePoints[0], trianglePoints[1]);

	triangleLines[0] = line0;
	triangleLines[1] = line1;
	triangleLines[2] = line2;


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

	determineLineSlopes();				// determine the X/Y/Z slopes for each TriangleLine
	determineCentroid();				// find centroid of the contoured triangle
	determinePointKeys();				// determine the point keys
	checkIfPointsAreInSameBlueprint();	// checks if points are in same blueprint; a OSContouredTriangleRunner will NOT call runContouredTriangleOriginalDirection(); if this is true.
	loadAndCalibrateKeyPairArray();		// calibrates, or adjusts the point keys of the ContouredTriangle 
										//-- i.e, if X = 32, we need to determine if we are in the blueprint having a Blueprint key X value of 0 or 1. 
										// (because 32 is shared between these)
	checkForPerfectClamping();			// sets the value of contouredTrianglePerfectClampValue
}

void OSContouredTriangle::determineCentroid()
{
	centroid = OrganicUtils::determineTriangleCentroid(trianglePoints[0], trianglePoints[1], trianglePoints[2]);
}
void OSContouredTriangle::determinePointKeys()
{
	for (int x = 0; x < 3; x++)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(trianglePoints[x].x);
		blueprintKey.y = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(trianglePoints[x].y);
		blueprintKey.z = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(trianglePoints[x].z);
		pointKeys[x] = blueprintKey;
	}
}

void OSContouredTriangle::checkForPerfectClamping()
{
	// x clamp check
	if
	(
		(triangleLines[0].pointA.x == triangleLines[1].pointA.x)
		&&
		(triangleLines[1].pointA.x == triangleLines[2].pointA.x)
	)
	{
		//std::cout << "Perfect X-clamp detected! " << std::endl;
		contouredTrianglePerfectClampValue = 1;			// 1 == clamped to X
	}

	// y clamp check
	if
	(
		(triangleLines[0].pointA.y == triangleLines[1].pointA.y)
		&&
		(triangleLines[1].pointA.y == triangleLines[2].pointA.y)
	)
	{
		//std::cout << "Perfect Y-clamp detected! " << std::endl;
		contouredTrianglePerfectClampValue = 2;			// 2 == clamped to X
	}

	// z clamp check
	if
	(
		(triangleLines[0].pointA.z == triangleLines[1].pointA.z)
		&&
		(triangleLines[1].pointA.z == triangleLines[2].pointA.z)
	)
	{
		//std::cout << "Perfect Z-clamp detected! " << std::endl;
		contouredTrianglePerfectClampValue = 3;			// 3 == clamped to X
	}
}

void OSContouredTriangle::addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceID)
{
	//std::cout << "!! Adding polygon piece: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << std::endl;
	//polygonPieceMap[in_Key] = in_pieceType;
	polygonPieceMap.emplace(in_Key, in_pieceID);
	//someVector.push_back(in_pieceType);
	//polygonPieceMap[in_Key] = 0;
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

void OSContouredTriangle::determineLineSlopes()
{
	//determineLineAxisIntercept(&triangleLines[0], trianglePoints[2]);
	//determineLineAxisIntercept(&triangleLines[1], trianglePoints[0]);
	//determineLineAxisIntercept(&triangleLines[2], trianglePoints[1]);

	PolyUtils::determineLineInterceptSlopes(&triangleLines[0], trianglePoints[2]);
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
			//std::cout << "############### Calibrating line: " << x << std::endl;
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

int OSContouredTriangle::isPerfectlyClamped()
{
	int result = 0;
	if (contouredTrianglePerfectClampValue != 0)	// if this triangle is clamped to ANYTHING, return 1.
	{
		result = 1;
	}
	return result;
}

int OSContouredTriangle::getPerfectClampValue()
{
	return contouredTrianglePerfectClampValue;
}

void OSContouredTriangle::adjustPointKeysIfPerfectlyAlignedToECBBorders()
{
	EnclaveKeyDef::EnclaveKey currentKeyCopy;
	ECBBorderLineList currentBorderLineList;
	for (int x = 0; x < 3; x++)
	{
		TriangleLine currentLine = triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &pointKeys[x];									// get a pointer to the key of the point
		currentKeyCopy = pointKeys[x];									// get a copy to the key of the point, to determine the original ECBBorderLineList from the pre-modified EnclaveKey of the point

		//std::cout << std::endl;
		//std::cout << "Key calibration; current key used is: " << currentKeyCopy.x << ", " << currentKeyCopy.y << ", " << currentKeyCopy.z << std::endl;

		currentBorderLineList = IndependentUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list	
		//std::cout << "################ Calibrating keys for line: " << x << std::endl;
		//findTrueKeysForTriangleLinePoints(contouredTrianglePtr, currentLine, currentKeyPtr, currentBorderLineList);	// calculate the true key for the points in the lines. This function call handles one point of the contoured triangle per call. (so loop 3 times)
	}

	// check for perfect clamps; we can use the last iteration of currentKeyCopy for this
	if (contouredTrianglePerfectClampValue == 1)		// if perfectly clamped to X... do this
	{
		/*

		The below logic is needed, because of the 32 = next rule in the blueprint system; For example, if the x of the points in question is 32, then
		this lies on the border of the blueprints at 0,0,0 and 1,0,0. If it's perfectly flat, we must check the direction of x that the center of the contour line lies in.

		*/
		std::cout << "######################## Perfect clamp detected, for X; attempting adjustment... " << std::endl;
		TriangleLine tempLine = triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.x == currentBorderLineList.corner_LowerNW.cornerPoint.x)		// check the triangle centroid, compare it to the center of the contour line 
		{
			/*
			if (planDirections.x_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].x -= 1;
					std::cout << "Key altered as a result of perfect clamping (X NEGATIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
		else if (tempLine.pointA.x == currentBorderLineList.corner_LowerNE.cornerPoint.x)
		{
			/*
			if (planDirections.x_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].x += 1;
					std::cout << "Key altered as a result of perfect clamping (X POSITIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
	}

	else if (contouredTrianglePerfectClampValue == 2)	// if perfectly clamped to Y... do this
	{
		TriangleLine tempLine = triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.y == currentBorderLineList.corner_LowerNW.cornerPoint.y)		// triangle is at very bottom
		{
			/*
			if (planDirections.y_direction == -1)	// if the clamped triangle is at the very bottom and plan direction is BELOW, shift all points by -1
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].y -= 1;
					std::cout << "Key altered as a result of perfect clamping (Y NEGATIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
		else if (tempLine.pointA.y == currentBorderLineList.corner_UpperNW.cornerPoint.y)	// triangle is at very top
		{
			/*
			if (planDirections.y_direction == 1)		// if the clamped triangle is at the very top and plan direction is ABOVE, shift all points by 1
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].y += 1;
					std::cout << "Key altered as a result of perfect clamping (Y POSITIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
	}

	else if (contouredTrianglePerfectClampValue == 3)		// if perfectly clamped to Z... do this
	{
		TriangleLine tempLine = triangleLines[0];
		if (tempLine.pointA.z == currentBorderLineList.corner_LowerNW.cornerPoint.z)		// triangle is at very bottom
		{
			/*
			if (planDirections.z_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].z -= 1;
					std::cout << "Key altered as a result of perfect clamping (Z NEGATIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
		else if (tempLine.pointA.z == currentBorderLineList.corner_LowerSW.cornerPoint.z)
		{
			/*
			if (planDirections.z_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					pointKeys[a].z += 1;
					std::cout << "Key altered as a result of perfect clamping (Z POSITIVE): " << pointKeys[a].x << ", " << pointKeys[a].y << ", " << pointKeys[a].z << std::endl;
				}
			}
			*/
		}
	}

}
