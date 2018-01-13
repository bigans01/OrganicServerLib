#include "stdafx.h"
#include "OSServer.h"
#include "OrganicUtils.h"
//#include "OSContourPlan.h"
//#include "shitTest.h"

OSServer::OSServer()
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
	organicSystemPtr = &Organic;
}

OSServer::OSServer(int x)
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
	organicSystemPtr = &Organic;
}

void OSServer::addContourPlan(string in_string, float in_x, float in_y, float in_z)
{
	OSContourPlan tempPlan(in_x, in_y, in_z);
	//contourPlanMap[in_string].planMode = 0;	// add a new plan			<<< ---- error is here (FIXED)
	contourPlanMap[in_string] = tempPlan;

}

void OSServer::executeContourPlan(string in_string)
{
	cout << "calling executeContourPlan... (3)" << endl;
	OSContourPlan* planPtr = &contourPlanMap[in_string];
	int numberOfTriangleStrips = planPtr->triangleStripMap.size();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapIterator = planPtr->triangleStripMap.begin();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapEnd = planPtr->triangleStripMap.end();

	
	// iterate through each strip

	/*
	for (stripMapIterator; stripMapIterator != stripMapEnd; stripMapIterator++)
	{
		//cout << "Triangle strip ID: " << stripMapIterator->first << endl;
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = stripMapIterator->second.triangleMap.end();
		// iterate through each triangle
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			//cout << "Current triangle ID: " << triangleMapIterator->first << endl;
			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			traceTriangleThroughBlueprints(currentTriangle);
		}
	}
	*/

	unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
	OSContouredTriangle* currentTriangle = &triangleMapIterator->second;

	traceTriangleThroughBlueprints(currentTriangle);

}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle)
{
	// STEP 1:  determine line lengths, angles
	in_Triangle->determineLineLengths();
	in_Triangle->determineLineAngles();

	CursorPathTraceContainer testContainer = OrganicUtils::getPreciseCoordinate(-32.2f);		// testing only; make sure that going +32.0f into a blueprint means default behavior is to go into the next blueprint in that direction
	cout << "container test: -----------" << endl;
	cout << "collection" << testContainer.CollectionCoord << endl;
	cout << "chunk" << testContainer.ChunkCoord << endl;
	cout << "block" << testContainer.BlockCoord << endl;

	EnclaveKeyTri currentTriKey;		// for usage with new function determineTriangleRelativityToECB
	ECBPolyPointTri currentTriPoint;


	// STEP 2: determine the blueprint each point is in: (a type 0 "piece");
	for (int x = 0; x < 3; x++)
	{
		
		CursorPathTraceContainer x_container, y_container, z_container;
		//x_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle->trianglePoints[x].x);		// get coords of the point at x (0, 1, 2)
		//y_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle->trianglePoints[x].y);
		//z_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle->trianglePoints[x].z);

		x_container = OrganicUtils::getPreciseCoordinate(in_Triangle->trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(in_Triangle->trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(in_Triangle->trianglePoints[x].z);

		
		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

		currentTriPoint.triPoints[x] = in_Triangle->trianglePoints[x];		// add this point and its assumed precise blueprint key
		currentTriKey.triKey[x] = blueprintKey;


		/*
		int doesBPExist = checkIfBlueprintExists(blueprintKey);
		if (doesBPExist == 0)
		{
			EnclaveCollectionBlueprint newBlueprint;
			blueprintMap[blueprintKey] = newBlueprint;											// set up the blueprint
			ECBCarvePointArray newArray;
			carvePointArrayMap[blueprintKey] = newArray;										// set up the server's carvePointArray
			ECBBorderValues newBorderValues = OrganicUtils::getBlueprintLimits(blueprintKey);	
			in_Triangle->ecbBorderMap[blueprintKey] = newBorderValues;							// add the ECB border map

			//carvePointArrayMap[blueprintKey].totalPointsInArray = 0;	// set up the server's carvePointArray
		}
		
		
		in_Triangle->addPolygonPiece(blueprintKey, 0);	// add the type 0 piece to the triangle's polygon piece map
		ECBCarvePointArray* carvePointArrayPtr = &carvePointArrayMap[blueprintKey];	 // grab a pointer
		
		unsigned char carvePointX = (x_container.ChunkCoord * 4) + x_container.BlockCoord;			// range should be between 0 and 31, only
		unsigned char carvePointY = (y_container.ChunkCoord * 4) + y_container.BlockCoord;			// ""
		unsigned char carvePointZ = (z_container.ChunkCoord * 4) + z_container.BlockCoord;			// ""
		
		carvePointArrayPtr->addECBCarvePoint(carvePointX, carvePointY, carvePointZ, 1);				// last argument = 1, testing only
		

		//carvePointArrayPtr->carvePointArray[]
		*/
		//y_container = EnclaveCollections.GetCursorCoordTrace(origin_point.y);
		//z_container = EnclaveCollections.GetCursorCoordTrace(origin_point.z);
		
	}

	// STEP 3: perform calibration checks (currently using test triangle only)
	OSContouredTriangle testTriangle;
	ECBPolyPoint testPoint_0, testPoint_1, testPoint_2;
	testPoint_0.x = 64.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 54.0f;
	testPoint_1.y = -1.0f;
	testPoint_1.z = -3.0f;

	testPoint_2.x = 69.0f;
	testPoint_2.y = 0.53f;
	testPoint_2.z = -10.0f;
	testTriangle.trianglePoints[0] = testPoint_0;
	testTriangle.trianglePoints[1] = testPoint_1;
	testTriangle.trianglePoints[2] = testPoint_2;
	testTriangle.determineLineLengths();
	testTriangle.determineLineAngles();
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

		//currentTriPoint.triPoints[x] = testTriangle.trianglePoints[x];		// add this point and its assumed precise blueprint key
		//currentTriKey.triKey[x] = blueprintKey;
		testTriangle.pointKeys[x] = blueprintKey;
	}

	auto bluestart = std::chrono::high_resolution_clock::now();
	determineTriangleRelativityToECB(&testTriangle);		// perform calibrations on this single contoured triangle, so that points of the triangle are in the appropriate EnclaveKey
	auto blueend = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> blueelapsed = blueend - bluestart;
	std::cout << "Elapsed time (Triangle calibration): " << blueelapsed.count() << std::endl;
}

void OSServer::determineTriangleRelativityToECB(OSContouredTriangle* in_Triangle)
{
	//cout << "Relativity job BEGIN ||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
	// step 1: check for Type 1 condition: 2 points of triangle that have a pair of the same coordinates (clamped to an axis)
	int conditionMetFlag = 0;			// determines what condition has been met
	for (int x = 0; x < 3; x++)
	{
		OSTriangleLine* linePtr = &in_Triangle->triangleLines[x];	// get reference to appropriate line
		int x_match = 0;					// indicates B.x- A.x = 0
		int y_match = 0;					// y
		int z_match = 0;					// z
		int conditionCount = 0;				// if this becomes 2, it is a Type 1 

		//  x coord check
		if ((linePtr->pointB.x - linePtr->pointA.x) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			x_match = 1;
		}

		// y
		if ((linePtr->pointB.y - linePtr->pointA.y) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			y_match = 1;
		}

		// z
		if ((linePtr->pointB.z - linePtr->pointA.z) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			z_match = 1;
		}
		
		if (conditionCount == 2)
		{
			cout << "TYPE 1 condition met. " << endl;
			// other new stuff
			if (y_match == 1 && z_match == 1)	// triangle pivots on x, perform checks
			{
				cout << "TYPE 1: pivots on X axis" << endl;
				// get y values of all points...check to see if its perfectly flat.
				if (
					(in_Triangle->trianglePoints[0].y == in_Triangle->trianglePoints[1].y)
					&&
					(in_Triangle->trianglePoints[1].y == in_Triangle->trianglePoints[2].y)
				   )
				{
					in_Triangle->isTriangleFlat = 1; // make sure to establish that its officially flat
					cout << "TYPE 1: x-pivot is perfectly flat...no angle!! " << endl;
				}

			}
			else if (x_match == 1 && y_match == 1)	// .... on y
			{
				cout << "TYPE 1: pivots on Z axis" << endl;
				if (
					(in_Triangle->trianglePoints[0].y == in_Triangle->trianglePoints[1].y)
					&&
					(in_Triangle->trianglePoints[1].y == in_Triangle->trianglePoints[2].y)
					)
				{
					in_Triangle->isTriangleFlat = 1; // make sure to establish that its officially flat
					cout << "TYPE 1: z-pivot is perfectly flat...no angle!! " << endl;
				}
			}
			else // .... otherwise, it's z
			
			{
				cout << "TYPE 1: pivots on Y axis" << endl;
				// no y-flat checks need to be done (because this is default elevation mode)
			}

			conditionMetFlag = 1;	// indicates condition 1 was met
			break;					// exit when 2 (condition 1 is met)
		}
	}

	// step 2: check for Type 2 condition: at least one point is on a border of the currently designated blueprint
	calibrateTrianglePointKeys(in_Triangle);

	//cout << "Relativity job END ||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
}

void OSServer::calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle)
{
	for (int x = 0; x < 3; x++)
	{
		OSTriangleLine currentLine = in_Triangle->triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &in_Triangle->pointKeys[x];									// get a pointer to the key of the point
		EnclaveKeyDef::EnclaveKey currentKeyCopy = in_Triangle->pointKeys[x];									// get a copy to the key of the point, to determine the original ECBBorderLineList from the pre-modified EnclaveKey of the point
		ECBBorderLineList currentBorderLineList = OrganicUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list
		
		findTrueKey(currentLine, currentKeyPtr, currentBorderLineList);	// calculate the true key for the point
	}
}

void OSServer::findTrueKey(OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList)
{
	EnclaveKeyDef::EnclaveKey calibratedKey;
	ECBPolyPoint pointToCheck = in_Line.pointA;
	//  check for x on the West side |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	/*
	if (pointToCheck.x == (in_KeyPtr->x * 32))	// if x is equal to the exact west border
	{
		// first, check the border points

		// is point equal to lower NW?
		if (pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y	&&		pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
		{
			cout << "Point is at lower NW..." << endl;
			//ECBPolyPoint passPoint = in_borderLineList.corner_LowerNW.cornerPoint;
			//cout << "Pointed to key, pre-calc:" << endl;
			//cout << "x: " << in_KeyPtr->x << endl;
			//cout << "y: " << in_KeyPtr->y << endl;
			//cout << "z: " << in_KeyPtr->z << endl;
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			//cout << "Pointed to key, post-calc:" << endl;
			//cout << "x: " << in_KeyPtr->x << endl;
			//cout << "y: " << in_KeyPtr->y << endl;
			//cout << "z: " << in_KeyPtr->z << endl;

		}
		else if (pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		else if (pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y   &&  pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		else if (pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		else
		{
			// if it's not within border points, check the lines themselves
			
			if (pointToCheck.y == (in_KeyPtr->y * 32))	// first: check for lower west line
			{
				cout << "Point is in the lower west line..." << endl;
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_lowerWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
				//cout << "Pointed to key, post-calc: " << endl;
				//cout << "x: " << in_KeyPtr->x << endl;
				//cout << "y: " << in_KeyPtr->y << endl;
				//cout << "z: " << in_KeyPtr->z << endl;
			}
			else if (pointToCheck.y == ((in_KeyPtr->y * 32) + 32))	// ... upper west
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_upperWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			else if (pointToCheck.z == (in_KeyPtr->z * 32))		// ... north West 
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_northWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			else if (pointToCheck.z == ((in_KeyPtr->z * 32) + 32))	// ... south West
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_southWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			else
			{
				// if it's not in a corner, and it's not in a border line, it's in the face
				cout << "Point is in the West face..." << endl;
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.WestFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
				
			}
		}
	}
	*/
	//  check for z on the North side |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



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


	//cout << "Pointed to key, post-calc::: " << endl;
	//cout << "x: " << in_KeyPtr->x << endl;
	//cout << "y: " << in_KeyPtr->y << endl;
	//cout << "z: " << in_KeyPtr->z << endl;

	//return calibratedKey;
}


int OSServer::checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key)
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintMapIterator;
	blueprintMapIterator = blueprintMap.find(in_Key);
	if (blueprintMapIterator != blueprintMap.end())
	{
		//cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	//cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

OSContourPlan* OSServer::getContourPlan(string in_string)
{
	return &contourPlanMap[in_string];		// return a pointer to the plan
}