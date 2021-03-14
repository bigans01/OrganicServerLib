#include "stdafx.h"
#include "OSContouredTriangleRunner.h"

void OSContouredTriangleRunner::performRun()
{
	rayCastTrianglePoints();
	//printTracingCounts();
	//std::cout << "##############################  Ray casting complete. " << std::endl;
	//std::cout << "!!!!!! Calling fillMetaDataInPrimartCircuits... " << std::endl;
	contouredTrianglePtr->fillMetaDataInPrimaryCircuits();
	//std::cout << "##############################  Primary circuit fill complete. " << std::endl;
	if (contouredTrianglePtr->containedWithinSameBlueprint == false)		// if the OSContouredTriangle isn't isolated to one blueprint, we'll have to trace it.
	{
		runContouredTriangleOriginalDirection();
		//runContouredTriangleReverseDirection();							// potentially unused, will need to be tested again (3/14/2021)
	}
	//prepareContouredTriangleData(PolyRunDirection::NORMAL);				// potentially unused, will need to be tested again (3/14/2021)
	// only do the following if all points are NOT in same blueprint
	contouredTrianglePtr->printPrimarySegmentData();
	//std::cout << "##############################  Run complete. " << std::endl;
}

/*
void OSContouredTriangleRunner::findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, TriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList)
{
	EnclaveKeyDef::EnclaveKey calibratedKey;
	//EnclaveKeyDef::EnclaveKey curentKey = *in_KeyPtr;
	//EnclaveKeyDef::EnclaveKey keyForBlueprintBorders = *in_KeyPtr;
	//ECBBorderValues borderValues = OrganicUtils::getBlueprintLimits(keyForBlueprintBorders);


	ECBPolyPoint pointToCheck = in_Line.pointA;
	//std::cout << "OrganicServerLib, pointToCheck: " << pointToCheck.x << ", " << pointToCheck.y << ", " << pointToCheck.z << ", " << std::endl;

	std::cout << "**** Point A: " << in_Line.pointA.x << ", " << in_Line.pointA.y << ", " << in_Line.pointA.z << std::endl;
	std::cout << "**** Point B: " << in_Line.pointB.x << ", " << in_Line.pointB.y << ", " << in_Line.pointB.z << std::endl;

	std::cout << "Pointed to key, pre-calc::: " << in_KeyPtr->x << ", " << in_KeyPtr->y << ", " << in_KeyPtr->z << std::endl;

	// NEW CODE BELOW |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// First, cycle through all 8 corner points: ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	if (pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x	&&		pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y	&&		pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)		// Lower NW
	{
		//cout << "Point is at lower NW..." << endl;
		//ECBPolyPoint passPoint = in_borderLineList.corner_LowerNW.cornerPoint;
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);

	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerSW.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)			// Lower SW
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower SW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperSW.cornerPoint.x   &&	pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y   &&  pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)			// Upper SW	 
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper SW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperNW.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)			// Upper NW
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper NW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x	&&  pointToCheck.y == in_borderLineList.corner_LowerNE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerNE.cornerPoint.z)			// Lower NE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerNE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower NE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerSE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_LowerSE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSE.cornerPoint.z)			// Lower SE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerSE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower SE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperSE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperSE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperSE.cornerPoint.z)			// Upper SE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperSE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper SE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperNE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperNE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNE.cornerPoint.z)			// Upper NE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperNE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper NE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else
	{

		// Second, cycle through all lines	||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

		// start with x axis lines:			

		// Lower North line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if ((pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_LowerNW.cornerPoint.x	&&		pointToCheck.x < in_borderLineList.corner_LowerNE.cornerPoint.x)	// is x between the LowerNW and LowerNE corners, but not equal to either of them?
			)
		{
			std::cout << "||||||| Lower north matched. " << std::endl;
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_lowerNorth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Lower South  line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_LowerSW.cornerPoint.x	&&	pointToCheck.x < in_borderLineList.corner_LowerSE.cornerPoint.x)		// is x between the LowerSW and LowerSE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_lowerSouth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper South line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_UpperSW.cornerPoint.x	&& pointToCheck.x < in_borderLineList.corner_UpperSE.cornerPoint.x)			// is x between the UpperSW and UpperSE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_upperSouth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper North line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_UpperNW.cornerPoint.x	&& pointToCheck.x < in_borderLineList.corner_UpperNE.cornerPoint.x)			// is x between the UpperNW and UpperNE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_upperNorth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}



		// now do z lines:

		// Lower West line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_LowerNW.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_LowerSW.cornerPoint.z)			// is z between the LowerNW and LowerSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_lowerWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper West line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_UpperNW.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_UpperNW.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_UpperSW.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_upperWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper East line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_UpperNE.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_UpperNE.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_UpperNE.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_UpperSE.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_upperEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// Lower East line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_LowerNE.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_LowerNE.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_LowerSE.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_lowerEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// now do y lines:

		// North East line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNE.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerNE.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperNE.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_northEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// South East line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerSE.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSE.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerSE.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperSE.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_southEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// South West line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerSW.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerSW.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperSW.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_southWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// North West line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerNW.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperNW.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_northWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		else
		{
			// Third, cycle through all faces ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

			// West face check
			if (pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.WestFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// North face check
			else if (pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.NorthFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// East face check
			else if (pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.EastFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// South face check
			else if (pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.SouthFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// Top face check
			else if (pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.TopFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// Bottom face check
			else if (pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.BottomFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}

		}

	}

	// check for influence from clamp flags in the line; if its clamped to x/y/z, check to see if it rotates on this line's axis in either negative or positive fashion (needs explanation/diagram)
	if ((in_Line.clamped_to_x == 1) && (in_Triangle->perfect_clamp_x == 0))
	{
		//cout << "x clamp entry" << endl;
		if ((in_Line.x_clamp_direction == 1) && (in_Line.pointA.x == in_borderLineList.corner_LowerNE.cornerPoint.x))
		{
			std::cout << "positive entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.x_clamp_direction == -1) && (in_Line.pointA.x == in_borderLineList.corner_LowerNW.cornerPoint.x))
		{
			std::cout << "negative entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}
	if ((in_Line.clamped_to_y == 1) && (in_Triangle->perfect_clamp_y == 0))
	{
		//cout << "y clamp entry" << endl;
		if ((in_Line.y_clamp_direction == 1) && (in_Line.pointA.y == in_borderLineList.corner_UpperNW.cornerPoint.y))
		{
			std::cout << "positive entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.y = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.y_clamp_direction == -1) && (in_Line.pointA.y == in_borderLineList.corner_LowerNW.cornerPoint.y))
		{
			std::cout << "negative entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.y = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}
	if ((in_Line.clamped_to_z == 1) && (in_Triangle->perfect_clamp_z == 0))
	{
		//cout << "z clamp entry" << endl;
		if ((in_Line.z_clamp_direction == 1) && (in_Line.pointA.z == in_borderLineList.corner_UpperSW.cornerPoint.z))
		{
			std::cout << "positive entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.z = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.z_clamp_direction == -1) && (in_Line.pointA.z == in_borderLineList.corner_UpperNW.cornerPoint.z))
		{
			std::cout << "negative entry " << std::endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.z = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}

	std::cout << "Pointed to key, post-calc::: " << in_KeyPtr->x << ", " << in_KeyPtr->y << ", " << in_KeyPtr->z << std::endl;
	//std::cout << "x: " << in_KeyPtr->x << std::endl;
	//std::cout << "y: " << in_KeyPtr->y << std::endl;
	//std::cout << "z: " << in_KeyPtr->z << std::endl;

	//return calibratedKey;
}
*/

void OSContouredTriangleRunner::rayCastTrianglePoints()
{
	//std::cout << "--------------------------------------------------------------------------------------------------------------------- !!! Attempting ray cast. " << std::endl;
	for (int x = 0; x < 3; x++)		// ray cast each individual line. (there will always be 3 lines -- duh)
	{
		//std::cout << "----------------------------------------Ray casting line: " << x << std::endl;
		tracePointThroughBlueprints(x);		// trace the point (line)
	}
	//std::cout << "!!! Ray cast complete. " << std::endl;
}

void OSContouredTriangleRunner::printContourTrianglePointsDebug()
{
	std::cout << "!! Printing contoured triangle points: " << std::endl;
	for (int x = 0; x < 3; x++)
	{
		std::cout << "[" << x << "] " << contouredTrianglePtr->trianglePoints[x].x << ", " << contouredTrianglePtr->trianglePoints[x].y << ", " << contouredTrianglePtr->trianglePoints[x].z << std::endl;
	}
	int someVal = 3;
	std::cin >> someVal;
}

void OSContouredTriangleRunner::tracePointThroughBlueprints(int in_pointID)
{
	//std::cout << "!!! Attempting trace. " << std::endl;
	EnclaveKeyDef::EnclaveKey originPointKey;
	EnclaveKeyDef::EnclaveKey endPointKey;

	EnclaveKeyPair currentPair = contouredTrianglePtr->keyPairArray[in_pointID].getBeginAndEndKeys();
	originPointKey = currentPair.keyA;
	endPointKey = currentPair.keyB;

	//std::cout << ":::: Begin Key of line: " << originPointKey.x << ", " << originPointKey.y << ", " << originPointKey.z << std::endl;
	//std::cout << ":::: End   Key of line: " << endPointKey.x << ", " << endPointKey.y << ", " << endPointKey.z << std::endl;


	EnclaveKeyDef::EnclaveKey incrementingKey = originPointKey;		// will constantly increment and/or decrement as it traverses blueprints
	//std::cout << "Incrementing Key is: " << incrementingKey.x << ", " << incrementingKey.y << ", " << incrementingKey.z << std::endl;

	if (originPointKey == endPointKey)		// both points exist in same blueprint
	{

		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = contouredTrianglePtr->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != contouredTrianglePtr->polygonPieceMap.end())	// polygon was already found
		{
			//std::cout << "!!! First branch hit. " << std::endl;
			int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map
			//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(incrementingKey)->second;	// get a pointer to the blueprint (for code readability only)
			//&(*blueprintMapRef)[currentKey];
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];
			contouredTrianglePtr->insertTracedBlueprint(incrementingKey);			// traced blueprint set update (in case it wasn't inserted already.
			adherenceOrderRef->attemptAdherentInsertion(incrementingKey);

			ECBPolyLine newPolyLine;												// create a new poly line
			OSServerUtils::fillLineMetaData(&newPolyLine, contouredTrianglePtr, in_pointID, contouredTrianglePtr->triangleLines[in_pointID].pointA,
				contouredTrianglePtr->triangleLines[in_pointID].pointB);
			contouredTrianglePtr->addNewPrimarySegment(newPolyLine.pointA, newPolyLine.pointB, in_pointID, incrementingKey);

			/*
			if (debugIncremental == 1)
			{
				std::cout << "Poly line points: " << std::endl;
				std::cout << "0: " << newPolyLine.pointA.x << ", " << newPolyLine.pointA.y << ", " << newPolyLine.pointA.z << std::endl;
				std::cout << "1: " << newPolyLine.pointB.x << ", " << newPolyLine.pointB.y << ", " << newPolyLine.pointB.z << std::endl;
				std::cout << "2: " << newPolyLine.pointC.x << ", " << newPolyLine.pointC.y << ", " << newPolyLine.pointC.z << std::endl;
				std::cout << "----Slopes: " << std::endl;
				std::cout << "X: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
				std::cout << "Y: " << newPolyLine.y_interceptSlope.x << ", " << newPolyLine.y_interceptSlope.y << ", " << newPolyLine.y_interceptSlope.z << std::endl;
				std::cout << "Z: " << newPolyLine.z_interceptSlope.x << ", " << newPolyLine.z_interceptSlope.y << ", " << newPolyLine.z_interceptSlope.z << std::endl;
			}
			*/
			blueprintPtr->primaryPolygonMap[polygonIDinBlueprint].lineMap[in_pointID] = newPolyLine;
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
		}
		else  // polygon wasn't found, it needs to be created
		{
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);	--reference this later? come back and look at it...(writing blueprints to disk)
			// ((INSERTING NEW POLY))
			ECBPoly newPoly(contouredTrianglePtr->contouredPolyType);
			newPoly.materialID = contouredTrianglePtr->materialID;
			newPoly.emptyNormal = contouredTrianglePtr->contouredEmptyNormal;
			OSServerUtils::fillPolyWithClampResult(&newPoly, contouredTrianglePtr);
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];

			contouredTrianglePtr->insertTracedBlueprint(incrementingKey);		// traced blueprint set update (in case it wasn't inserted already.
			adherenceOrderRef->attemptAdherentInsertion(incrementingKey);
			
			OSServerUtils::analyzePolyValidityAndInsert(contouredTrianglePtr,
				contouredTrianglePtr->triangleLines[in_pointID].pointA,
				contouredTrianglePtr->triangleLines[in_pointID].pointB,
				in_pointID,
				incrementingKey,
				&borderDataMapInstance,
				blueprintPtr,
				&newPoly);
													
			
			/*
			int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
			blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
			ECBPolyLine newPolyLine;												// create a new poly line
			OSServerUtils::fillLineMetaData(&newPolyLine, contouredTrianglePtr, in_pointID, contouredTrianglePtr->triangleLines[in_pointID].pointA, contouredTrianglePtr->triangleLines[in_pointID].pointB);
			
			contouredTrianglePtr->addNewPrimarySegment(newPolyLine.pointA, newPolyLine.pointB, in_pointID, incrementingKey);
			blueprintPtr->primaryPolygonMap[elementID].lineMap[in_pointID] = newPolyLine;
			contouredTrianglePtr->addPolygonPiece(incrementingKey, elementID);
			runnerForgedPolyRegistryRef->addToPolyset(incrementingKey, elementID); // Add the new poly to the ForgedPolyRegistry
			*/
		}

	}
	else								// points do not exist in same blueprint; run a OSTriangleLineTraverser
	{
		OSTriangleLineTraverser lineTraverser(contouredTrianglePtr, in_pointID, blueprintMapRef, adherenceOrderRef);
		OSTriangleLineTraverser* lineRef = &lineTraverser;
		while (!(lineRef->currentKey == lineRef->endKey))
		{
			lineRef->traverseLineOnce(contouredTrianglePtr);

		}

	}
}

void OSContouredTriangleRunner::runContouredTriangleOriginalDirection()
{
	//int sillyVal;
	//std::cout << "enter the silly val. " << std::endl;
	//std::cin >> sillyVal;

	//int stupid = 3;
	//while (stupid == 0)
	//{

	//}

	//std::cout << "########################## Running Original Direction ################################# " << std::endl;
	PrimaryLineT1Array contourLineArray;
	prepareContouredTriangleData(PolyRunDirection::NORMAL, &contourLineArray);
	fillBlueprintArea(&contourLineArray);

	//std::cout << "!!! Original direction run complete..." << std::endl;
}

void OSContouredTriangleRunner::runContouredTriangleReverseDirection()
{
	//std::cout << "########################## Running Reverse Direction ################################# " << std::endl;
	PrimaryLineT1Array contourLineArray;
	prepareContouredTriangleData(PolyRunDirection::REVERSE, &contourLineArray);
	fillBlueprintArea(&contourLineArray);
}

void OSContouredTriangleRunner::prepareContouredTriangleData(PolyRunDirection in_direction, PrimaryLineT1Array* in_contourLineArrayRef)
{
	//int sillyVal;
	//std::cout << "enter the silly val. " << std::endl;
	//std::cin >> sillyVal;

	//int perfectClampFlag = isContouredTrianglePerfectlyClamped();		// is the contoured triangle perfectly clamped?
	int perfectClampFlag = contouredTrianglePtr->isPerfectlyClamped();
	for (int x = 0; x < 3; x++)		// go through each line in the contoured triangle.
	{
		

		ECBPolyPoint pointA = contouredTrianglePtr->triangleLines[x].pointA;
		ECBPolyPoint pointB = contouredTrianglePtr->triangleLines[x].pointB;
		ECBPolyPoint pointC = contouredTrianglePtr->triangleLines[x].pointC;
		PrimaryLineT1 newPrimaryLine;
		newPrimaryLine.IDofLine = x;
		newPrimaryLine.perfectClampValue = perfectClampFlag;
		newPrimaryLine.isLineActiveInPoly = 1;

		//EnclaveKeyPair pointBlueprintKeys = getBlueprintKeysForPrimaryLinePoints(x);	// get the CALIBRATED blueprint keys for the points
		EnclaveKeyPair pointBlueprintKeys = contouredTrianglePtr->keyPairArray[x].getBeginAndEndKeys();
		newPrimaryLine.beginPointBlueprintKey = pointBlueprintKeys.keyA;
		newPrimaryLine.endPointBlueprintKey = pointBlueprintKeys.keyB;

		newPrimaryLine.beginPointRealXYZ = pointA;				// store actual XYZ values of point A
		newPrimaryLine.endPointRealXYZ = pointB;				// store actual XYZ values of point B
		newPrimaryLine.thirdPointRealXYZ = pointC;				// store actual XYZ values of point C (not always needed)
		newPrimaryLine.intendedFaces = OrganicUtils::determineIntendedFacesV2(pointA, pointB, pointC, contouredTrianglePtr->triangleLines[x].x_interceptSlope, contouredTrianglePtr->triangleLines[x].y_interceptSlope, contouredTrianglePtr->triangleLines[x].z_interceptSlope);
		newPrimaryLine.x_int = contouredTrianglePtr->triangleLines[x].x_interceptSlope;
		newPrimaryLine.y_int = contouredTrianglePtr->triangleLines[x].y_interceptSlope;
		newPrimaryLine.z_int = contouredTrianglePtr->triangleLines[x].z_interceptSlope;
		//std::cout << "!!!!! Points for line are: " << std::endl;
		//std::cout << "pointA: " << pointA.x << ", " << pointA.y << ", " << pointA.z << std::endl;
		//std::cout << "pointB: " << pointB.x << ", " << pointB.y << ", " << pointB.z << std::endl;
		//std::cout << "pointC: " << pointC.x << ", " << pointC.y << ", " << pointC.z << std::endl;

		//newPrimaryLine.calibrateBlueprintKeys(pointC);

		if (in_direction == PolyRunDirection::NORMAL)
		{

		}
		else if (in_direction == PolyRunDirection::REVERSE)
		{
			newPrimaryLine.calibrateForBlueprintTracingWithInverseChecks(pointC);
		}
		in_contourLineArrayRef->addNewPrimaryLine(newPrimaryLine);

		
	}

	// check for line swap
	if (in_direction == PolyRunDirection::REVERSE)
	{
		in_contourLineArrayRef->swapLinesForBlueprintTracing();
	}
	//in_contourLineArrayRef->printBlueprintTracingLines();

	//std::cout << "!!! prepareContouredTrianglePerfectlyClamped complete..." << std::endl;
	
}

void OSContouredTriangleRunner::fillBlueprintArea(PrimaryLineT1Array* in_contourLineArrayRef)
{
	for (int x = 0; x < 3; x++)		// attempt fill through each line
	{
		OSBlueprintTraversalController traversalController(in_contourLineArrayRef->linkArray[x], in_contourLineArrayRef->linkArray[x].IDofLine);
		if (traversalController.isLineContainedToOneBlueprint() == false)
		{
			//std::cout << "!!!" << std::endl;
			//std::cout << "!!! Beginning traversal for line: " << x << std::endl;
			traversalController.blueprintTraverser.initialize(&in_contourLineArrayRef->linkArray[x]);		// initialize with the line from the primary t1 array



			// begin while looping here
			//  ...
			// EX: while (OSBlueprintTraversalController.OSTriangleLineTraverserBare->isTraversalComplete == 0) etc...
			//    {
			//         do this if there's 1 line
			//         --else
			//	       do this if there's 2 lines
			//    }
			while (traversalController.blueprintTraverser.isRunComplete == 0)
			{
				//std::cout << "!!! Executing one traversal iteration for line " << x << std::endl;
				traversalController.blueprintTraverser.checkIfRunComplete();
				EnclaveKeyDef::EnclaveKey currentKey = traversalController.blueprintTraverser.currentKey;
				int traceCount = contouredTrianglePtr->tracedBlueprintCountMap[currentKey];
				//std::cout << "The currently traced blueprint (Key: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << ") has had " << traceCount << " primary lines go through it. " << std::endl;

				if (traceCount == 1)
				{
					// spawn a new primary line from THIS primary line; see "PrimaryLineT2IsolatedTracer" for reference in OrganicPolyOperationsLib.
					//std::cout << "Initialized runner, for trace count 1 " << std::endl;
					BlueprintFillerRunner fillerRunner;
					fillerRunner.initialize(&in_contourLineArrayRef->linkArray[x], 
						                    traversalController.blueprintTraverser.currentIterationBeginPoint, 
											traversalController.blueprintTraverser.currentIterationEndpoint, 
											currentKey, 
											&contouredTrianglePtr->tracedBlueprintCountMap,
											&contouredTrianglePtr->filledBlueprintMap, 
											contouredTrianglePtr, 
											blueprintMapRef,
											adherenceOrderRef);
				}
				else if (traceCount == 2)
				{
					/*
					bool isOnBorder = checkIfPointIsOnBlueprintBorder(traversalController.blueprintTraverser.currentIterationEndpoint, currentKey);		// only proceed if the endpoint of the current line is on a border
					if (isOnBorder == true)
					{
						BlueprintFillerRunner fillerRunner;
						fillerRunner.initialize(&in_contourLineArrayRef->linkArray[x], traversalController.blueprintTraverser.currentIterationBeginPoint, traversalController.blueprintTraverser.currentIterationEndpoint, currentKey, &contouredTrianglePtr->tracedBlueprintCountMap, &contouredTrianglePtr->filledBlueprintMap, contouredTrianglePtr, blueprintMapRef);
					}
					*/
					if (contouredTrianglePtr->primarySegmentTrackerMap[currentKey].currentSegmentCount == 2)
					{
						//std::cout << "::::: Primary segment tracker has 2 segments!! " << std::endl;
						//std::cout << "Checking if line " << int(in_contourLineArrayRef->linkArray[x].IDofLine) << " is OPEN_MULTI..." << std::endl;
						PrimaryCircuit* circuitRef = &contouredTrianglePtr->primarySegmentTrackerMap[currentKey];
						if (circuitRef->checkIfSegmentIsOpenMulti(in_contourLineArrayRef->linkArray[x].IDofLine) == true)
						{
							//std::cout << "This line is OPEN_MULTI!!! " << std::endl;
							//std::cout << "Initialized runner, for trace count 2 " << std::endl;
							BlueprintFillerRunner fillerRunner;
							fillerRunner.initialize(&in_contourLineArrayRef->linkArray[x], 
													traversalController.blueprintTraverser.currentIterationBeginPoint, 
													traversalController.blueprintTraverser.currentIterationEndpoint, 
													currentKey, 
													&contouredTrianglePtr->tracedBlueprintCountMap, 
													&contouredTrianglePtr->filledBlueprintMap, 
													contouredTrianglePtr, 
													blueprintMapRef,
													adherenceOrderRef);
						}
					}
				}
				traversalController.blueprintTraverser.traverseLineOnce();
				//std::cout << "!!! Traversed to new blueprint.... " << std::endl;
			}
		
		}
		else
		{
			//std::cout << "+++" << std::endl;
		}
	}
}

bool OSContouredTriangleRunner::checkIfPointIsOnBlueprintBorder(ECBPolyPoint in_point, EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	bool check = false;
	ECBBorderLineList currentBorderLineList = OrganicUtils::determineBorderLines(in_blueprintKey);
	ECBPPOrientationResults currentEndpointOrientationResults = OrganicUtils::GetBlueprintPointOrientation(in_point, &currentBorderLineList);
	if (currentEndpointOrientationResults.osubtype != ECBPPOrientations::NOVAL)
	{
		check = true;
		std::cout << "!!!! >>>> Point " << in_point.x << ", " << in_point.y << ", " << in_point.z << " is on a border" << std::endl;
	}
	return check;
}

void OSContouredTriangleRunner::printTracingCounts()
{
	auto traceBegin = contouredTrianglePtr->tracedBlueprintCountMap.begin();
	auto traceEnd = contouredTrianglePtr->tracedBlueprintCountMap.end();
	for (traceBegin; traceBegin != traceEnd; traceBegin++)
	{
		//std::cout << "Traced Key: (" << traceBegin->first.x << ", " << traceBegin->first.y << ", " << traceBegin->first.z << ") -> " << traceBegin->second << std::endl;
	}
}

EnclaveKeyPair OSContouredTriangleRunner::getBlueprintKeysForPrimaryLinePoints(int in_lineID)
{
	EnclaveKeyPair returnPair;
	if (in_lineID < 2)
	{
		returnPair.keyA = contouredTrianglePtr->pointKeys[in_lineID];
		returnPair.keyB = contouredTrianglePtr->pointKeys[in_lineID + 1];
	}
	else if (in_lineID == 2)
	{
		returnPair.keyA = contouredTrianglePtr->pointKeys[2];
		returnPair.keyB = contouredTrianglePtr->pointKeys[0];
	}
	return returnPair;
}