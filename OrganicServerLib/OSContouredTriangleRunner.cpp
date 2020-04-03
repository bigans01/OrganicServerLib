#include "stdafx.h"
#include "OSContouredTriangleRunner.h"

void OSContouredTriangleRunner::performRun()
{
	checkForPerfectClamping();			// check for any perfect clamping conditions.
	calibrateTrianglePointKeys();		// perform key calibration; adjusts keys if any perfect clamping conditions are met.
	rayCastTrianglePoints();

	printTracingCounts();

	if (contouredTrianglePtr->checkIfPointsAreInSameBlueprint() == false)
	{
		runContouredTriangleOriginalDirection();
	}
	//prepareContouredTriangleData(PolyRunDirection::NORMAL);

	// only do the following if all points are NOT in same blueprint
}

void OSContouredTriangleRunner::checkForPerfectClamping()
{
	if ((contouredTrianglePtr->triangleLines[0].clamped_to_x == 1) && (contouredTrianglePtr->triangleLines[1].clamped_to_x == 1) && (contouredTrianglePtr->triangleLines[2].clamped_to_x == 1))
	{
		std::cout << "triangle is perfectly clamped to x!" << std::endl;
		contouredTrianglePtr->perfect_clamp_x = 1;
	}
	if ((contouredTrianglePtr->triangleLines[0].clamped_to_y == 1) && (contouredTrianglePtr->triangleLines[1].clamped_to_y == 1) && (contouredTrianglePtr->triangleLines[2].clamped_to_y == 1))
	{
		std::cout << "triangle is perfectly clamped to y!" << std::endl;
		contouredTrianglePtr->perfect_clamp_y = 1;
	}
	if ((contouredTrianglePtr->triangleLines[0].clamped_to_z == 1) && (contouredTrianglePtr->triangleLines[1].clamped_to_z == 1) && (contouredTrianglePtr->triangleLines[2].clamped_to_z == 1))
	{
		std::cout << "triangle is perfectly clamped to z!" << std::endl;
		contouredTrianglePtr->perfect_clamp_z = 1;
	}
}

void OSContouredTriangleRunner::calibrateTrianglePointKeys()
{
	EnclaveKeyDef::EnclaveKey currentKeyCopy;
	ECBBorderLineList currentBorderLineList;
	for (int x = 0; x < 3; x++)
	{
		TriangleLine currentLine = contouredTrianglePtr->triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &contouredTrianglePtr->pointKeys[x];									// get a pointer to the key of the point
		currentKeyCopy = contouredTrianglePtr->pointKeys[x];									// get a copy to the key of the point, to determine the original ECBBorderLineList from the pre-modified EnclaveKey of the point
		currentBorderLineList = OrganicUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list	
		findTrueKeysForTriangleLinePoints(contouredTrianglePtr, currentLine, currentKeyPtr, currentBorderLineList);	// calculate the true key for the points in the lines. This function call handles one point of the contoured triangle per call. (so loop 3 times)
	}

	// check for perfect clamps; we can use the last iteration of currentKeyCopy for this
	if (contouredTrianglePtr->perfect_clamp_x == 1)
	{
		/*

		The below logic is needed, because of the 32 = next rule in the blueprint system; For example, if the x of the points in question is 32, then
		this lies on the border of the blueprints at 0,0,0 and 1,0,0. If it's perfectly flat, we must check the direction of x that the center of the contour line lies in.

		*/

		TriangleLine tempLine = contouredTrianglePtr->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.x == currentBorderLineList.corner_LowerNW.cornerPoint.x)		// check the triangle centroid, compare it to the center of the contour line 
		{
			if (planDirections.x_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].x -= 1;
					std::cout << "Key altered as a result of perfect clamping (X NEGATIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
		else if (tempLine.pointA.x == currentBorderLineList.corner_LowerNE.cornerPoint.x)
		{
			if (planDirections.x_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].x += 1;
					std::cout << "Key altered as a result of perfect clamping (X POSITIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
	}

	else if (contouredTrianglePtr->perfect_clamp_y == 1)
	{
		TriangleLine tempLine = contouredTrianglePtr->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.y == currentBorderLineList.corner_LowerNW.cornerPoint.y)		// triangle is at very bottom
		{
			if (planDirections.y_direction == -1)	// if the clamped triangle is at the very bottom and plan direction is BELOW, shift all points by -1
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].y -= 1;
					std::cout << "Key altered as a result of perfect clamping (Y NEGATIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
		else if (tempLine.pointA.y == currentBorderLineList.corner_UpperNW.cornerPoint.y)	// triangle is at very top
		{
			if (planDirections.y_direction == 1)		// if the clamped triangle is at the very top and plan direction is ABOVE, shift all points by 1
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].y += 1;
					std::cout << "Key altered as a result of perfect clamping (Y POSITIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
	}

	else if (contouredTrianglePtr->perfect_clamp_z == 1)
	{
		TriangleLine tempLine = contouredTrianglePtr->triangleLines[0];
		if (tempLine.pointA.z == currentBorderLineList.corner_LowerNW.cornerPoint.z)		// triangle is at very bottom
		{
			if (planDirections.z_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].z -= 1;
					std::cout << "Key altered as a result of perfect clamping (Z NEGATIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
		else if (tempLine.pointA.z == currentBorderLineList.corner_LowerSW.cornerPoint.z)
		{
			if (planDirections.z_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					contouredTrianglePtr->pointKeys[a].z += 1;
					std::cout << "Key altered as a result of perfect clamping (Z POSITIVE): " << contouredTrianglePtr->pointKeys[a].x << ", " << contouredTrianglePtr->pointKeys[a].y << ", " << contouredTrianglePtr->pointKeys[a].z << std::endl;
				}
			}
		}
	}

}

void OSContouredTriangleRunner::findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, TriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList)
{
	EnclaveKeyDef::EnclaveKey calibratedKey;
	//EnclaveKeyDef::EnclaveKey curentKey = *in_KeyPtr;
	//EnclaveKeyDef::EnclaveKey keyForBlueprintBorders = *in_KeyPtr;
	//ECBBorderValues borderValues = OrganicUtils::getBlueprintLimits(keyForBlueprintBorders);


	ECBPolyPoint pointToCheck = in_Line.pointA;
	//std::cout << "OrganicServerLib, pointToCheck: " << pointToCheck.x << ", " << pointToCheck.y << ", " << pointToCheck.z << ", " << std::endl;
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

	//cout << "Pointed to key, post-calc::: " << endl;
	//cout << "x: " << in_KeyPtr->x << endl;
	//cout << "y: " << in_KeyPtr->y << endl;
	//cout << "z: " << in_KeyPtr->z << endl;

	//return calibratedKey;
}

void OSContouredTriangleRunner::rayCastTrianglePoints()
{
	for (int x = 0; x < 3; x++)		// ray cast each individual line. (there will always be 3 lines -- duh)
	{
		tracePointThroughBlueprints(x);		// trace the point (line)
	}
}

void OSContouredTriangleRunner::tracePointThroughBlueprints(int in_pointID)
{
	EnclaveKeyDef::EnclaveKey originPointKey;
	EnclaveKeyDef::EnclaveKey endPointKey;

	if (in_pointID < 2)
	{
		originPointKey = contouredTrianglePtr->pointKeys[in_pointID];
		endPointKey = contouredTrianglePtr->pointKeys[in_pointID + 1];
	}
	else if (in_pointID == 2)
	{
		originPointKey = contouredTrianglePtr->pointKeys[2];
		endPointKey = contouredTrianglePtr->pointKeys[0];
	}

	EnclaveKeyDef::EnclaveKey incrementingKey = originPointKey;		// will constantly increment and/or decrement as it traverses blueprints

	if (originPointKey == endPointKey)		// both points exist in same blueprint
	{
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = contouredTrianglePtr->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != contouredTrianglePtr->polygonPieceMap.end())	// polygon was already found
		{

			int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map
			//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(incrementingKey)->second;	// get a pointer to the blueprint (for code readability only)
			//&(*blueprintMapRef)[currentKey];
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];
			contouredTrianglePtr->insertTracedBlueprint(incrementingKey);			// traced blueprint set update (in case it wasn't inserted already.
			ECBPolyLine newPolyLine;												// create a new poly line
			fillLineMetaData(&newPolyLine, in_pointID);
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

			ECBPoly newPoly;
			newPoly.materialID = contouredTrianglePtr->materialID;
			//fillPolyWithClampResult(&newPoly, contouredTrianglePtr);
			//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(incrementingKey)->second;
			//
			//&(*blueprintMapRef)[currentKey];
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];
			contouredTrianglePtr->insertTracedBlueprint(incrementingKey);		// traced blueprint set update (in case it wasn't inserted already.
			int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
			blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
			ECBPolyLine newPolyLine;												// create a new poly line
			fillLineMetaData(&newPolyLine, in_pointID);
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
			blueprintPtr->primaryPolygonMap[elementID].lineMap[in_pointID] = newPolyLine;
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
			contouredTrianglePtr->addPolygonPiece(incrementingKey, elementID);

		}

	}
	else								// points do not exist in same blueprint
	{
		//cout << "The begin point for line " << in_pointID << " is NOT in same area as its endpoint" << endl;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = contouredTrianglePtr->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != contouredTrianglePtr->polygonPieceMap.end())	// polygon was already found
		{
			//std::cout << "++++++++++++Branch 1 hit (begin) " << std::endl;
			int blueprintIDofFoundPolygon = polyMapIter->second;											// get the corresponding int value from the triangle's registered blueprint polygon map
			//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(incrementingKey)->second;	// get a pointer to the blueprint (for code readability only)
			//&(*blueprintMapRef)[currentKey];
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];
			//contouredTrianglePtr->insertTracedBlueprint(incrementingKey);						// traced blueprint set update (in case it wasn't inserted already.

			OSTriangleLineTraverser lineTraverser(contouredTrianglePtr, in_pointID, blueprintMapRef);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			//std::cout << "++++++++++++Branch 1 hit (mid)" << std::endl;
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				//std::cout << "Traversal call...." << std::endl;
				lineRef->traverseLineOnce(contouredTrianglePtr);
				//std::cout << "traversing once...!!!" << std::endl;
			}
			


		}
		else  // polygon wasn't found, it needs to be created
		{
			//std::cout << "Branch 2 hit" << std::endl;
			ECBPoly newPoly;
			newPoly.materialID = contouredTrianglePtr->materialID;
			fillPolyWithClampResult(&newPoly);
			//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(incrementingKey)->second;
			//&(*blueprintMapRef)[currentKey];
			EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[incrementingKey];
			//contouredTrianglePtr->insertTracedBlueprint(incrementingKey);					// traced blueprint set update (in case it wasn't inserted already.							

			int currentBlueprintPolyMapSize = blueprintPtr->primaryPolygonMap.size();
			blueprintPtr->primaryPolygonMap[currentBlueprintPolyMapSize] = newPoly;		// insert a new polygon; the ID will be equalto the size
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
			contouredTrianglePtr->addPolygonPiece(incrementingKey, currentBlueprintPolyMapSize);
			
			OSTriangleLineTraverser lineTraverser(contouredTrianglePtr, in_pointID, blueprintMapRef);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				lineRef->traverseLineOnce(contouredTrianglePtr);
				//std::cout << "traversing once...!!!" << std::endl;
			}
			
		}
		//cout << "Ending this section..." << std::endl;

	}
}

void OSContouredTriangleRunner::fillLineMetaData(ECBPolyLine* in_LinePtr, int in_pointID)
{
	//in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = contouredTrianglePtr->triangleLines[in_pointID].pointA;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = contouredTrianglePtr->triangleLines[in_pointID].pointB;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = contouredTrianglePtr->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = contouredTrianglePtr->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = contouredTrianglePtr->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = contouredTrianglePtr->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = contouredTrianglePtr->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = contouredTrianglePtr->triangleLines[in_pointID].z_interceptSlope;		// "" z
}

void OSContouredTriangleRunner::fillPolyWithClampResult(ECBPoly* in_polyPtr)
{
	if (contouredTrianglePtr->perfect_clamp_x == 1
		||
		contouredTrianglePtr->perfect_clamp_y == 1
		||
		contouredTrianglePtr->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 1;
	}
}

void OSContouredTriangleRunner::runContouredTriangleOriginalDirection()
{
	PrimaryLineT1Array contourLineArray;
	prepareContouredTriangleData(PolyRunDirection::NORMAL, &contourLineArray);
	fillBlueprintArea(&contourLineArray);
}

void OSContouredTriangleRunner::prepareContouredTriangleData(PolyRunDirection in_direction, PrimaryLineT1Array* in_contourLineArrayRef)
{
	int perfectClampFlag = isContouredTrianglePerfectlyClamped();		// is the contoured triangle perfectly clamped?
	for (int x = 0; x < 3; x++)		// go through each line in the contoured triangle.
	{
		ECBPolyPoint pointA = contouredTrianglePtr->triangleLines[x].pointA;
		ECBPolyPoint pointB = contouredTrianglePtr->triangleLines[x].pointB;
		ECBPolyPoint pointC = contouredTrianglePtr->triangleLines[x].pointC;
		PrimaryLineT1 newPrimaryLine;
		newPrimaryLine.IDofLine = x;
		newPrimaryLine.perfectClampValue = perfectClampFlag;
		newPrimaryLine.isLineActiveInPoly = 1;

		EnclaveKeyPair pointBlueprintKeys = getBlueprintKeysForPrimaryLinePoints(x);	// get the CALIBRATED blueprint keys for the points
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
}

void OSContouredTriangleRunner::fillBlueprintArea(PrimaryLineT1Array* in_contourLineArrayRef)
{
	for (int x = 0; x < 3; x++)		// attempt fill through each line
	{
		OSBlueprintTraversalController traversalController(in_contourLineArrayRef->linkArray[x], in_contourLineArrayRef->linkArray[x].IDofLine);
		if (traversalController.isLineContainedToOneBlueprint() == false)
		{
			//std::cout << "!!!" << std::endl;
			std::cout << "!!! Beginning traversal for line: " << x << std::endl;
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
					BlueprintFillerRunner fillerRunner;
					fillerRunner.initialize(&in_contourLineArrayRef->linkArray[x]);
				}
				else if (traceCount == 2)
				{

				}
				traversalController.blueprintTraverser.traverseLineOnce();
			}

		}
		else
		{
			//std::cout << "+++" << std::endl;
		}
	}
}

int OSContouredTriangleRunner::isContouredTrianglePerfectlyClamped()
{
	int result = 0;
	if (contouredTrianglePtr->perfect_clamp_x == 1
		||
		contouredTrianglePtr->perfect_clamp_y == 1
		||
		contouredTrianglePtr->perfect_clamp_z == 1)
	{
		result = 1;
	}
	return result;
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