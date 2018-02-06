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

void OSServer::addContourPlan(string in_planName, OSPDir in_Dir, float in_x, float in_y, float in_z)
{
	OSContourPlan tempPlan(in_Dir, in_x, in_y, in_z);
	//contourPlanMap[in_string].planMode = 0;	// add a new plan			<<< ---- error is here (FIXED)
	contourPlanMap[in_planName] = tempPlan;

}

void OSServer::executeContourPlan(string in_string)
{
	cout << "calling executeContourPlan... (3)" << endl;
	OrganicUtils::findAdjacentUsingCAH(35.0f, 4.00048f);
	OrganicUtils::findHypotenuseUsingCAH(35.0f, 3.277f);
	OrganicUtils::findAngleUsingCAH(3.277f, 4.00048f);		// 3.277, 4.00048			// 6.554,	8.00096

	OrganicUtils::findOppositeUsingSOH(35.0f, 4.00048f);
	OrganicUtils::findHypotenuseUsingSOH(35.0f, 2.29458f);
	OrganicUtils::findAngleUsingSOH(2.29458f, 4.00048f);
	//OrganicUtils::findAngleUsingCAH(6.554, 8.00096);		// 3.277, 4.00048			// 6.554,	8.00096
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

	traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections);

}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	// STEP T-1:  determine line lengths, angles
	in_Triangle->determineLineLengths();
	in_Triangle->determineLineAngles();
	//in_Triangle->determineAxisInterceptDistances();

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
	//testPoint_0.x = 64.0f;
	//testPoint_0.y = 0.0f;
	//testPoint_0.z = 0.0f;

	//testPoint_1.x = 28.0f;
	//testPoint_1.y = 0.0f;		// previously: -1.3f
	//testPoint_1.z = -3.0f;

	//testPoint_2.x = 69.0f;
	//testPoint_2.y = 33.0f;		// previously: 1.0f
	//testPoint_2.z = -10.0f;



	// X == Y test
	testPoint_0.x = 48.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 16.0f;

	testPoint_1.x = 16.0f;
	testPoint_1.y = -16.0f;		// previously: -1.3f
	testPoint_1.z = 49.0f;

	testPoint_2.x = 69.0f;
	testPoint_2.y = 33.0f;		// previously: 1.0f
	testPoint_2.z = -10.0f;



	// X == Z test
	testPoint_0.x = 48.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 16.0f;

	testPoint_1.x = 16.0f;
	testPoint_1.y = -15.8f;		// previously: -1.3f
	testPoint_1.z = 48.0f;

	testPoint_2.x = 69.0f;
	testPoint_2.y = 33.0f;		// previously: 1.0f
	testPoint_2.z = -10.0f;


	// Rotation test
	testPoint_0.x = 4.0f;
	testPoint_0.y = 28.0f;
	testPoint_0.z = 10.0f;

	testPoint_1.x = 35.0f;
	testPoint_1.y = 18.0f;
	testPoint_1.z = 12.0f;

	testPoint_2.x = 10.0f;
	testPoint_2.y = 10.0f;
	testPoint_2.z = 13.5f;


	testTriangle.trianglePoints[0] = testPoint_0;
	testTriangle.trianglePoints[1] = testPoint_1;
	testTriangle.trianglePoints[2] = testPoint_2;
	testTriangle.determineLineLengths();
	testTriangle.determineLineAngles();
	testTriangle.determineAxisInterceptDistances();
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
	cout << ">>>>>beginning determineTriangleRelativityToECB" << endl;
	auto bluestart = std::chrono::high_resolution_clock::now();
	determineTriangleRelativityToECB(&testTriangle, in_Directions);		// perform calibrations on this single contoured triangle, so that points of the triangle are in the appropriate EnclaveKey
	auto blueend = std::chrono::high_resolution_clock::now();
	cout << ">>>>>ending determineTriangleRelativityToECB" << endl;
	std::chrono::duration<double> blueelapsed = blueend - bluestart;
	std::cout << "Elapsed time (Triangle calibration)::: " << blueelapsed.count() << std::endl;
}

void OSServer::determineTriangleCentroid(OSContouredTriangle* in_Triangle)
{

}

void OSServer::determineTriangleRelativityToECB(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	//cout << "Relativity job BEGIN ||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
	// step 1: check for Type 1 condition: 2 points of triangle that have a pair of the same coordinates (clamped to an axis)
	int conditionMetFlag = 0;			// determines what condition has been met

	// used for this function only
	struct lineMeta
	{
		int line_ID = 0;				// for the loop
		OSTriangleLine* structLinePtr;	// ptr to the actual line
		ECBPolyPoint thirdPoint;		// = to point B of next line in the sequence
		int is_clamped = 0;				// is this line clamped?
	};

	lineMeta tempMeta[3];				// set up the struct

	for (int x = 0; x < 3; x++)
	{
		tempMeta[x].line_ID = x;
		tempMeta[x].structLinePtr = &in_Triangle->triangleLines[x];	
		if (x < 2)
		{
			tempMeta[x].thirdPoint = in_Triangle->triangleLines[x + 1].pointB;		// store the next point in the sequence
		}
		else if (x == 2)
		{
			tempMeta[x].thirdPoint = in_Triangle->triangleLines[0].pointB;		// store the first point in the sequence
		}



		OSTriangleLine* linePtr = &in_Triangle->triangleLines[x];	// get reference to appropriate line
		int x_match = 0;					// indicates B.x- A.x = 0
		int y_match = 0;					// y
		int z_match = 0;					// z
		int conditionCount = 0;				// if this becomes 2, it is a Type 1 


		//  x coord check
		if ((linePtr->pointB.x - linePtr->pointA.x) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			cout << "X check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_x = 1;
			if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) != 0)
			{
				cout << "X shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) > 0)
				{
					cout << "X difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_x = 1;
					tempMeta[x].structLinePtr->x_clamp_direction = 1;
				}
				else if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) < 0)
				{
					cout << "X difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_x = 1;
					tempMeta[x].structLinePtr->x_clamp_direction = -1;
				}
			}
			x_match = 1;
		}

		// y
		if ((linePtr->pointB.y - linePtr->pointA.y) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			cout << "Y check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_y = 1;			// indicate the line is clamped to y
			if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) != 0)
			{
				cout << "Y shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) > 0)
				{
					cout << "Y difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_y = 1;				// signal this line is clamped to y
					tempMeta[x].structLinePtr->y_clamp_direction = 1;			// positive slope = positive y
				}
				else if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) < 0)
				{
					cout << "Y difference is negative" << endl;
					tempMeta[x].structLinePtr->clamped_to_y = 1;				
					tempMeta[x].structLinePtr->y_clamp_direction = -1;			
				}
			}
			y_match = 1;
		}

		// z
		if ((linePtr->pointB.z - linePtr->pointA.z) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			//cout << "Z check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_z = 1;
			if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) != 0)
			{
				//cout << "Z shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) > 0)
				{
					//cout << "Z difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_z = 1;
					tempMeta[x].structLinePtr->z_clamp_direction = 1;
				}
				else if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) < 0)
				{
					//cout << "Z difference is negative" << endl;
					tempMeta[x].structLinePtr->clamped_to_z = 1;
					tempMeta[x].structLinePtr->z_clamp_direction = -1;
				}
			}
			z_match = 1;
		}
		
		/*
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
		else if (conditionCount == 1)
		{

		*/
	}

	// Step 2: check if triangle is perfectly clamped to x, y, or z (Type 2 condition)
	if ((in_Triangle->triangleLines[0].clamped_to_x == 1) && (in_Triangle->triangleLines[1].clamped_to_x == 1) && (in_Triangle->triangleLines[2].clamped_to_x == 1))
	{
		cout << "triangle is perfectly clamped to x!" << endl;
		in_Triangle->perfect_clamp_x = 1;
	}
	if ((in_Triangle->triangleLines[0].clamped_to_y == 1) && (in_Triangle->triangleLines[1].clamped_to_y == 1) && (in_Triangle->triangleLines[2].clamped_to_y == 1))
	{
		cout << "triangle is perfectly clamped to y!" << endl;
		in_Triangle->perfect_clamp_y = 1;
	}
	if ((in_Triangle->triangleLines[0].clamped_to_z == 1) && (in_Triangle->triangleLines[1].clamped_to_z == 1) && (in_Triangle->triangleLines[2].clamped_to_z == 1))
	{
		cout << "triangle is perfectly clamped to z!" << endl;
		in_Triangle->perfect_clamp_z = 1;
	}


	// step 3: calibrate point keys, after conditions have been checked

	// STEP T-2
	calibrateTrianglePointKeys(in_Triangle, in_Directions);

	//cout << "Relativity job END ||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;

	// step 4: begin ray cast sequence
	//for (int x = 0; x < 340; x++)
	//{

		// STEP T-3
		rayCastTrianglePoints(in_Triangle);
	//}

	// STEP T-4

}

void OSServer::rayCastTrianglePoints(OSContouredTriangle* in_Triangle)
{
	//cout << "ray cast test" << endl;
	// loop through each point

	for (int x = 0; x < 3; x++)
	{
		//cout << "Key value of point:  " << in_Triangle->pointKeys[x].x << ", " << in_Triangle->pointKeys[x].y << ", " << in_Triangle->pointKeys[x].z << endl;
		int checkResult = checkIfBlueprintExists(in_Triangle->pointKeys[x]);	// returns 0 if it didn't exist, 1 if so
		if (checkResult == 0)		// create a blueprint if it doesnt exist already
		{
			//EnclaveCollectionBlueprint newBlueprint;
			//cout << "ray cast test" << endl;
			//ECBPoly newPoly;
			//ECBPolyLine newPolyLine;
			//newPolyLine.pointA = in_Triangle->triangleLines[x].pointA;		// set the new line to the pointed-to point A
			//newPolyLine.pointB = in_Triangle->triangleLines[x].pointB;		// set the new line to the pointed-to point B
			// need angle set here
			// need angle set here
			//newBlueprint.polygonMap[0] = newPoly;							
			//blueprintMap[in_Triangle->pointKeys[x]] = newBlueprint;			// insert the new blueprint
			//in_Triangle->polygonPieceMap[in_Triangle->pointKeys[x]] = 0;	// since it's a new blueprint, the first element in polygonPieceMap will be 0

			// STEP T-3.2
			tracePointThroughBlueprints(in_Triangle, x);					// trace point (line) through blueprints
			//cout << "Blueprint didn't exist for polygon insertion, creating with polygonPieceMap insert at element 0" << endl;
		}
		else if (checkResult == 1)	// blueprint already exists
		{
			// STEP T-3.2
			tracePointThroughBlueprints(in_Triangle, x);

			/*
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(in_Triangle->pointKeys[x]);	// check to see if the polygon exists already in the contoured triangle
			if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
			{
				int blueprintIDofFoundPolygon = polyMapIter->second;	// get the corresponding int value from the triangle's registered blueprint polygon map
				EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[in_Triangle->pointKeys[x]];	// get a pointer to the blueprint (for code readability only)
				//ECBPolyLine newPolyLine;
				//newPolyLine.pointA = in_Triangle->triangleLines[x].pointA;		// set the new line to the pointed-to point A
				//newPolyLine.pointB = in_Triangle->triangleLines[x].pointB;		// set the new line to the pointed-to point B
				// need angle set here
				// need angle set here
				//blueprintPtr->polygonMap[blueprintIDofFoundPolygon].lineMap[x] = newPolyLine;
				cout << "Blueprint existed for polygon insertion. " << endl;
			}
			else  // wasn't found
			{

			}
			*/
		}
	}
}

void OSServer::tracePointThroughBlueprints(OSContouredTriangle* in_Triangle, int in_pointID)
{

	// STEP 1:	set appropriate values of keys 
	EnclaveKeyDef::EnclaveKey originPointKey;
	EnclaveKeyDef::EnclaveKey endPointKey;

	if (in_pointID < 2)
	{
		originPointKey = in_Triangle->pointKeys[in_pointID];
		endPointKey = in_Triangle->pointKeys[in_pointID + 1];
	}
	else if (in_pointID == 2)
	{
		originPointKey = in_Triangle->pointKeys[2];
		endPointKey = in_Triangle->pointKeys[0];
	}
	EnclaveKeyDef::EnclaveKey incrementingKey = originPointKey;		// incrementing key will constantly increment and/or decrement as it traverses blueprints


	// STEP 2: initiating tracing
	if (originPointKey == endPointKey)		// both points exist in same blueprint
	{
		//cout << "The begin point for line " << in_pointID << " is in same area as its endpoint" << endl;
		cout << "-----------origin and end point the same ------------------" << endl;
		cout << "Begin point value is: " << incrementingKey.x << ", " << incrementingKey.y << ", " << incrementingKey.z << ", " << endl;
		cout << "End point value is: " << endPointKey.x << ", " << endPointKey.y << ", " << endPointKey.z << ", " << endl;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
		{
			
			int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];	// get a pointer to the blueprint (for code readability only)
			ECBPolyLine newPolyLine;												// create a new poly line
			newPolyLine.pointA = in_Triangle->triangleLines[in_pointID].pointA;		// set the new line to the pointed-to point A
			newPolyLine.pointB = in_Triangle->triangleLines[in_pointID].pointB;		// set the new line to the pointed-to point B
			blueprintPtr->polygonMap[polygonIDinBlueprint].lineMap[in_pointID] = newPolyLine;
			
		}
		else  // polygon wasn't found, it needs to be created
		{
			
			ECBPoly newPoly;
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];
			int elementID = blueprintPtr->polygonMap.size();						// will store the ID of the newly inserted polygon
			blueprintPtr->polygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
			ECBPolyLine newPolyLine;												// create a new poly line
			newPolyLine.pointA = in_Triangle->triangleLines[in_pointID].pointA;		// set the new line to the pointed-to point A
			newPolyLine.pointB = in_Triangle->triangleLines[in_pointID].pointB;		// set the new line to the pointed-to point B
			blueprintPtr->polygonMap[elementID].lineMap[in_pointID] = newPolyLine;
			
		}
		
	}
	else								// points do not exist in same blueprint
	{
		//cout << "The begin point for line " << in_pointID << " is NOT in same area as its endpoint" << endl;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
		{
			int blueprintIDofFoundPolygon = polyMapIter->second;											// get the corresponding int value from the triangle's registered blueprint polygon map
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];	// get a pointer to the blueprint (for code readability only)
			cout << "-----------------------------" << endl;
			cout << "Begin point value is: " << incrementingKey.x << ", " << incrementingKey.y << ", " << incrementingKey.z << ", " << endl;
			cout << "End point value is: " << endPointKey.x << ", " << endPointKey.y << ", " << endPointKey.z << ", " << endl;

			//EnclaveKeyDef::EnclaveKey stupidKey;
			//EnclaveKeyDef::EnclaveKey *stupidKeyPtr = &stupidKey;

			OSTriangleLineTraverser lineTraverser(in_Triangle, in_pointID, this);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			/*
			while (!(incrementingKey == endPointKey))			// 	&&		(incrementingKey.y != endPointKey.y)		&&		(incrementingKey.z != endPointKey.z)
			{ 
				cout << "whoops! oh mannn!" << endl;
				if (endPointKey.x > incrementingKey.x)
				{
					incrementingKey.x += 1;
				}
				else if (endPointKey.x < incrementingKey.x)
				{
					incrementingKey.x -= 1;
				}
			}
			*/
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				lineRef->traverseLineOnce(in_Triangle);
				//lineRef->traverseLineOnce(in_Triangle);
			}
			

			
		}
		else  // polygon wasn't found, it needs to be created
		{
			ECBPoly newPoly;
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];
			cout << "-----------------------------" << endl;
			cout << "Begin point value is: " << incrementingKey.x << ", " << incrementingKey.y << ", " << incrementingKey.z << ", " << endl;
			cout << "End point value is: " << endPointKey.x << ", " << endPointKey.y << ", " << endPointKey.z << ", " << endl;
			blueprintPtr->polygonMap[blueprintPtr->polygonMap.size()] = newPoly;		// insert a new polygon; the ID will be equalto the size


			OSTriangleLineTraverser lineTraverser(in_Triangle, in_pointID, this);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			/*
			while (!(incrementingKey == endPointKey))			// && (incrementingKey.y != endPointKey.y) && (incrementingKey.z != endPointKey.z)
			{
				cout << "whoops! oh mannn!" << endl;
				if (endPointKey.x > incrementingKey.x)
				{
					incrementingKey.x += 1;
				}
				else if (endPointKey.x < incrementingKey.x)
				{
					incrementingKey.x -= 1;
				}
			}
			*/
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				lineRef->traverseLineOnce(in_Triangle);
				//lineRef->traverseLineOnce(in_Triangle);
			}
		}
	}
}

void OSServer::calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	EnclaveKeyDef::EnclaveKey currentKeyCopy;
	ECBBorderLineList currentBorderLineList;
	for (int x = 0; x < 3; x++)
	{
		OSTriangleLine currentLine = in_Triangle->triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &in_Triangle->pointKeys[x];									// get a pointer to the key of the point
		currentKeyCopy = in_Triangle->pointKeys[x];									// get a copy to the key of the point, to determine the original ECBBorderLineList from the pre-modified EnclaveKey of the point
		currentBorderLineList = OrganicUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list
		
		findTrueKey(in_Triangle, currentLine, currentKeyPtr, currentBorderLineList);	// calculate the true key for the point
	}

	// check for perfect clamps; we can use the last iteration of currentKeyCopy for this
	if (in_Triangle->perfect_clamp_x == 1)
	{
		/*
		
		The below logic is needed, because of the 32 = next rule in the blueprint system; For example, if the x of the points in question is 32, then 
		this lies on the border of the blueprints at 0,0,0 and 1,0,0. If it's perfectly flat, we must check the direction of x that the center of the contour line lies in.
		
		*/

		OSTriangleLine tempLine = in_Triangle->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.x == currentBorderLineList.corner_LowerNW.cornerPoint.x)		// check the triangle centroid, compare it to the center of the contour line 
		{
			if (in_Directions.x_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].x -= 1;
					cout << "Key altered as a result of perfect clamping (X NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.x == currentBorderLineList.corner_LowerNE.cornerPoint.x)
		{
			if (in_Directions.x_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].x += 1;
					cout << "Key altered as a result of perfect clamping (X POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

	else if (in_Triangle->perfect_clamp_y == 1)
	{
		OSTriangleLine tempLine = in_Triangle->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.y == currentBorderLineList.corner_LowerNW.cornerPoint.y)		// triangle is at very bottom
		{
			if (in_Directions.y_direction == -1)	// if the clamped triangle is at the very bottom and plan direction is BELOW, shift all points by -1
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].y -= 1;
					cout << "Key altered as a result of perfect clamping (Y NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.y == currentBorderLineList.corner_UpperNW.cornerPoint.y)	// triangle is at very top
		{
			if (in_Directions.y_direction == 1)		// if the clamped triangle is at the very top and plan direction is ABOVE, shift all points by 1
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].y += 1;
					cout << "Key altered as a result of perfect clamping (Y POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

	else if (in_Triangle->perfect_clamp_z == 1)
	{
		OSTriangleLine tempLine = in_Triangle->triangleLines[0];
		if (tempLine.pointA.z == currentBorderLineList.corner_LowerNW.cornerPoint.z)		// triangle is at very bottom
		{
			if (in_Directions.z_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].z -= 1;
					cout << "Key altered as a result of perfect clamping (Z NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.z == currentBorderLineList.corner_LowerSW.cornerPoint.z)
		{
			if (in_Directions.z_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].z += 1;
					cout << "Key altered as a result of perfect clamping (Z POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

}

void OSServer::findTrueKey(OSContouredTriangle* in_Triangle, OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList)
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

	// check for influence from clamp flags in the line; if its clamped to x/y/z, check to see if it rotates on this line's axis in either negative or positive fashion (needs explanation/diagram)
	if ((in_Line.clamped_to_x == 1) && (in_Triangle->perfect_clamp_x == 0))
	{
		cout << "x clamp entry" << endl;
		if ((in_Line.x_clamp_direction == 1)	&&	(in_Line.pointA.x == in_borderLineList.corner_LowerNE.cornerPoint.x))	
		{
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.x_clamp_direction == -1) && (in_Line.pointA.x == in_borderLineList.corner_LowerNW.cornerPoint.x)) 
		{
			cout << "negative entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}
	if ((in_Line.clamped_to_y == 1) && (in_Triangle->perfect_clamp_y == 0))
	{
		cout << "y clamp entry" << endl;
		if ((in_Line.y_clamp_direction == 1) && (in_Line.pointA.y == in_borderLineList.corner_UpperNW.cornerPoint.y))		
		{
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.y = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.y_clamp_direction == -1) && (in_Line.pointA.y == in_borderLineList.corner_LowerNW.cornerPoint.y))	
		{
			cout << "negative entry " << endl;
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
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.z = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.z_clamp_direction == -1) && (in_Line.pointA.z == in_borderLineList.corner_UpperNW.cornerPoint.z))
		{
			cout << "negative entry " << endl;
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