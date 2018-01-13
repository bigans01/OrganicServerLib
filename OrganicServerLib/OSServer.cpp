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
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 10.0f;
	testPoint_1.y = -1.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 5.0f;
	testPoint_2.y = 0.0f;
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


	determineTriangleRelativityToECB(&testTriangle);		// perform calibrations on this single contoured triangle, so that points of the triangle are in the appropriate EnclaveKey

}

void OSServer::determineTriangleRelativityToECB(OSContouredTriangle* in_Triangle)
{
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
}

void OSServer::calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle)
{
	for (int x = 0; x < 3; x++)
	{
		OSTriangleLine currentLine = in_Triangle->triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &in_Triangle->pointKeys[x];									// get a pointer to the key of the point
		EnclaveKeyDef::EnclaveKey currentKeyCopy = in_Triangle->pointKeys[x];									// get a copy to the key of the point
		ECBBorderLineList currentBorderLineList = OrganicUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list
		EnclaveKeyDef::EnclaveKey trueKey = findTrueKey(currentLine, currentKeyCopy, currentBorderLineList);	// calculate the true key for the point
	}
}

EnclaveKeyDef::EnclaveKey OSServer::findTrueKey(OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey in_KeyPtr, ECBBorderLineList in_borderLineList)
{
	EnclaveKeyDef::EnclaveKey calibratedKey;
	ECBPolyPoint pointToCheck = in_Line.pointA;
	// check for x on the West side
	if (pointToCheck.x == (in_KeyPtr.x * 32))	// if x is equal to the exact west border
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
			//*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			//cout << "Pointed to key, post-calc:" << endl;
			//cout << "x: " << in_KeyPtr->x << endl;
			//cout << "y: " << in_KeyPtr->y << endl;
			//cout << "z: " << in_KeyPtr->z << endl;

		}
		else if (pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
		{

		}
		else if (pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y   &&  pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)
		{

		}
		else if (pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)
		{

		}
	}

	return calibratedKey;
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