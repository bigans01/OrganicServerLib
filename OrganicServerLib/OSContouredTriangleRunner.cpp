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
			
			bool wasInserted = OSServerUtils::analyzePolyValidityAndInsert(contouredTrianglePtr,
				contouredTrianglePtr->triangleLines[in_pointID].pointA,
				contouredTrianglePtr->triangleLines[in_pointID].pointB,
				in_pointID,
				incrementingKey,
				&borderDataMapInstance,
				blueprintPtr,
				&newPoly);
													
			
			/*
			int elementID = blueprintPtr->fetchNextECBPolyKeyID();						// will store the ID of the newly inserted polygon
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
	ECBBorderLineList currentBorderLineList = IndependentUtils::determineBorderLines(in_blueprintKey);
	ECBPPOrientationResults currentEndpointOrientationResults = IndependentUtils::GetBlueprintPointOrientation(in_point, &currentBorderLineList);
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