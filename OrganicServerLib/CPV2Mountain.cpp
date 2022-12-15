#include "stdafx.h"
#include "CPV2Mountain.h"

void CPV2Mountain::initialize(DoublePoint in_startPoint,			
	int in_numberOfLayers,
	float in_distanceBetweenLayers,
	float in_startRadius,
	float in_expansionValue)
{
	numberOfLayers = in_numberOfLayers;
	setMountainUpperLayersContourPoints(in_startPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
	setMRPsForBottomLayers();
	setMountainLowerLayersContourPoints(bottomStartPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
}

void CPV2Mountain::amplifyAllContourLinePoints()
{
	// we could use either the upperContourLineCircuits or bottomContourLineCircuits for this, as
	// they'd each have the exact same number of contour lines.
	int numberOfLayers = upperContourLineCircuits.size();
	for (int x = 0; x < numberOfLayers; x++)
	{
		amplifyContourLinePoints(x);
	}
}

void CPV2Mountain::buildContouredTriangles()
{
	for (int x = 0; x < numberOfLayers; x++)
	{
		constructMountainTopStripTriangles(x);
	}
	for (int x = 0; x < numberOfLayers; x++)
	{
		constructMountainBottomStripTriangles(x);
	}
}

void CPV2Mountain::constructMountainTopStripTriangles(int in_stripID)
{
	constructStripTrianglesForLayer(&upperContourLineCircuits, &triangleUpperStripMRPMap, in_stripID, getPreferredMaterialAtIndex(0), &typicalShellStrips, ECBPolyType::SHELL, startPoint);
}

void CPV2Mountain::constructMountainBottomStripTriangles(int in_stripID)
{
	constructStripTrianglesForLayer(&bottomContourLineCircuits, &triangleBottomStripMRPMap, in_stripID, getPreferredMaterialAtIndex(1), &shellMassDriverStrips, ECBPolyType::SHELL_MASSDRIVER, bottomStartPoint);
}

void CPV2Mountain::constructStripTrianglesForLayer(std::map<int, ContouredCircuit>* in_circuitMapRef,
									std::map<int, DoublePoint>* in_triangleStripMRPMapRef,
									int in_stripID,
									TriangleMaterial in_materialID,
									std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
									ECBPolyType in_polyType,
									DoublePoint in_startPoint)
{
	// the very first strip is treated differently from the other strips; 
	// there are only 4 triangles that are constructed, period. So we loop 3 times for the first 3, then do special logic for the las one.
	if (in_stripID == 0)
	{	
		ContouredCircuit* firstCircuitRef = &(*in_circuitMapRef)[0];
		int numberOfPoints = firstCircuitRef->numberOfPoints;
		DoublePoint currentContourCenter = firstCircuitRef->circuitCenterPoint;
		DoublePoint massReferencePoint = (*in_triangleStripMRPMapRef)[in_stripID];	// get the MRP for strip at ID 0.

		// For the first 3 triangles, pointTwo and pointThree come directly from the firstCircuitRef;
		// the 1st point will always be the summit of the mountain.
		for (int x = 0; x < numberOfPoints - 1; x++)
		{
			DoublePoint pointOne = in_startPoint;
			DoublePoint pointTwo = firstCircuitRef->smartContourPoint[x].getDoublePoint();
			DoublePoint pointThree = firstCircuitRef->smartContourPoint[x + 1].getDoublePoint();

			// build the current triangle
			constructTriangle(in_contouredTriangleStripRef,
							pointOne,
							pointTwo,
							pointThree,
							massReferencePoint,
							0,
							in_materialID,
							in_polyType);
		}

		// do the following for the last triangle only; because we are coming to the end of the circuit,
		// point two will be final point in the circuit before it resets, and point 3 will be the point at index 0.
		int finalPointOne = numberOfPoints - 1;
		DoublePoint pointOne = in_startPoint;
		DoublePoint pointTwo = firstCircuitRef->smartContourPoint[finalPointOne].getDoublePoint();
		DoublePoint pointThree = firstCircuitRef->smartContourPoint[0].getDoublePoint();

		constructTriangle(in_contouredTriangleStripRef,
						pointOne,
						pointTwo,
						pointThree,
						massReferencePoint,
						0,
						in_materialID,
						in_polyType);
	}

	// other wise, we'll have to use quadrant shell production logic.
	else
	{
		ContouredCircuit* currentLineRef = &(*in_circuitMapRef)[in_stripID];	// a reference to the current line, so we can access the points.
		ContouredCircuit* previousLineRef = &(*in_circuitMapRef)[in_stripID - 1];

		int currentLineNumberOfPoints = currentLineRef->numberOfPoints;	// the number of points in the previous, or "upper" circuit
		DoublePoint currentContourCenter = currentLineRef->circuitCenterPoint;
		DoublePoint massReferencePoint = (*in_triangleStripMRPMapRef)[in_stripID];

		int pointsPerQuadrantCurrentLine = (currentLineNumberOfPoints / 4) + 1;	// get the number of points per quadrant,  for the current (lower) line;
																				// the number per quadrant should always be equal to the number from the previous line, plus 1.
		int trianglesForCurrentLine = pointsPerQuadrantCurrentLine - 1;	// the number of triangles in each quadrant is equal to the number of points per quadrant - 1

		// outer quadrants setup
		constructOuterQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructOuterQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructOuterQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructOuterQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_polyType);

		// inner quadrants setup
		constructInnerQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructInnerQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructInnerQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_polyType);
		constructInnerQuadrantShell(in_contouredTriangleStripRef, currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_polyType);
	}
}

void CPV2Mountain::constructOuterQuadrantShell(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
											ContouredCircuit* in_currentCircuit,
											ContouredCircuit* in_previousCircuit,
											int in_pointsPerQuadrant,
											int in_quadrantID,
											int in_triangleStripID,
											TriangleMaterial in_materialID,
											DoublePoint in_mrp,
											ECBPolyType in_type)
{
	if (in_quadrantID != 3)		// don't do this for the last quadrant
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1));		// get the point ID on the current line to start at, which is based on the quadrant id
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 1);			// the number of triangles to produce in the outer shell

		// produce the triangles

		for (int x = 0; x < numberOfTriangles; x++)
		{
			DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();
			DoublePoint secondPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint();
			DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine + 1].getDoublePoint();

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""

		}
	}

	else if (in_quadrantID == 3)
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1));		// get the point ID on the current line to start at, which is based on the quadrant id
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfNormalTriangles = (in_pointsPerQuadrant - 1) - 1;	// we need special logic for the very last triangle
		for (int x = 0; x < numberOfNormalTriangles; x++)
		{
			DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();
			DoublePoint secondPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint();
			DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine + 1].getDoublePoint();

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}
		
		// perform logic for final triangle; because we are doing the last triangle in a circuit,
		// the first point needs to be used, i.e, whatever is at index 0 in the previous line.
		basePointForPreviousLine = 0;
		DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();	// get the first point from the previous line
		DoublePoint secondPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint();	// get the second point from the current line
		DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[0].getDoublePoint();	// get the third point from the current line

		constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
	}
}

void CPV2Mountain::constructInnerQuadrantShell(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
											ContouredCircuit* in_currentCircuit,
											ContouredCircuit* in_previousCircuit,
											int in_pointsPerQuadrant,
											int in_quadrantID,
											int in_triangleStripID,
											TriangleMaterial in_materialID,
											DoublePoint in_mrp,
											ECBPolyType in_type)
{
	if (in_quadrantID != 3)		// don't do this for the last quadrant
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1)) + 1;		// get the point ID on the current line to start at, which is based on the quadrant id; because this is an inner shell, we increment base point by 1
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 2);			// the number of triangles to produce in the inner shell; will be equal to points per quadrant - 2.

		for (int x = 0; x < numberOfTriangles; x++)
		{
			DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();		// get the first point from the previous line
			DoublePoint secondPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine + 1].getDoublePoint();	// get the second point from the previous line
			DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint();	// get the third point from the current line	

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}


	}

	else if (in_quadrantID == 3)
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1)) + 1;		// get the point ID on the current line to start at, which is based on the quadrant id; because this is an inner shell, we increment base point by 1
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 2) - 1;			// the number of triangles to produce in the inner shell; will be equal to points per quadrant - 2.


		for (int x = 0; x < numberOfTriangles; x++)
		{
			DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();	// get the first point from the previous line
			DoublePoint secondPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine + 1].getDoublePoint();	// get the second point from the previous line
			DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint(); // get the third point from the base of the current line

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}


		// perform logic for final triangle
		DoublePoint firstPoint = in_previousCircuit->smartContourPoint[basePointForPreviousLine].getDoublePoint();	// get the first point from the previous line
		DoublePoint secondPoint = in_previousCircuit->smartContourPoint[0].getDoublePoint();
		DoublePoint thirdPoint = in_currentCircuit->smartContourPoint[basePointForCurrentLine].getDoublePoint();	// get the third point from the current line

		//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
		constructTriangle(in_contouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
	}
}

void CPV2Mountain::amplifyContourLinePoints(int in_lineID)
{
	upperContourLineCircuits[in_lineID].amplifyAllPoints();
	bottomContourLineCircuits[in_lineID].amplifyAllPoints();
}

void CPV2Mountain::constructTriangle(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
									DoublePoint in_point0,
									DoublePoint in_point1,
									DoublePoint in_point2,
									DoublePoint in_massReferencePoint,
									int in_triangleStripID,
									TriangleMaterial in_materialID,
									ECBPolyType in_polyType)
{
	ContouredTriangleV2 newTriangle(in_point0, 
									in_point1, 
									in_point2, 
									in_materialID, 
									in_massReferencePoint,
									&allPolysRegistry,
									&shellMassDriverRegistry,
									in_polyType);
	// Below: get the index in the triangle strip, that we will use to insert the triangle we just made.
	int currentStripTriangleIndex = (*in_contouredTriangleStripRef)[in_triangleStripID].triangleMap.size();
	(*in_contouredTriangleStripRef)[in_triangleStripID].triangleMap[currentStripTriangleIndex] = newTriangle;
}

void CPV2Mountain::runMassDriversV2(OrganicClient* in_clientRef,
	ECBMap* in_ecbMapRef,
	EnclaveFractureResultsMap* in_fractureResultsMapRef)
{

	// The below code block is the same as running multiple blank contours; this was just a "smoke test" to stamp out
	// any initial bugs, and to ensure that the plan V2 logic (i.e, using FTriangles to form ECBPolys) was working.
	// The logic to fill blueprints will come later.
	OrganicTriangleTracker oreTracker;
	for (auto& planPolyRegistryBegin : allPolysRegistry.polySetRegistry)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin.first;					// get the key of the blueprint to check.
		int foundGroupID = planPolyRegistryBegin.second.groupID;									// grab the group ID we'll be working with.
		std::cout << "Found poly set " << foundGroupID << "in key: (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
		EnclaveCollectionBlueprint* blueprintToCheck = in_ecbMapRef->getBlueprintRef(blueprintKey);	// get a ref to the blueprint that exists SERVER side (not on the client), using the blueprintKey
		auto forgedPolySetBegin = planPolyRegistryBegin.second.polySet.begin();
		auto forgedPolySetEnd = planPolyRegistryBegin.second.polySet.end();

		ForgedPolySet originalSet = allPolysRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set

		std::cout << "!! Size of the originalSet to be used in this blueprint: " << originalSet.polySet.size() << std::endl;

		EnclaveFractureResultsMap tempMap;
		in_clientRef->OS->produceRawEnclavesForPolySetWithTracking(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet, &oreTracker);		// first, generate the OrganicRawEnclaves that would be produced by this set; keep track of each ORE that an individual OrganicTriangle touches (needed for SPoly post-collision check)
		in_clientRef->OS->spawnAndAppendEnclaveTriangleSkeletonsToBlueprint(blueprintKey, &tempMap, blueprintToCheck, &oreTracker);
	}
}

void CPV2Mountain::setMountainUpperLayersContourPoints(DoublePoint in_startPoint,
	int in_numberOfLayers,
	float in_distanceBetweenLayers,
	float in_startRadius,
	float in_expansionValue)
{
	startPoint = in_startPoint;			// set the summit of the mountain
	double currentY = in_startPoint.y;	// the currentY always starts as the Y value of the summit of the mountain, but can get decremented multiple times.
	int currentNumberOfPoints = 4;				// first layer always starts with 4 points (for now, this will probably change later)
	float currentRadius = in_startRadius;		// set the start radius
	for (int x = 0; x < in_numberOfLayers; x++)
	{
		currentY -= in_distanceBetweenLayers;	// subtract the distance between layers for each layer being added
		DoublePoint mrpToAdd(in_startPoint);
		mrpToAdd.y = currentY;
		mrpToAdd.roundHundredth();	// round to hundredth, as a rule

		bottomStartPoint = mrpToAdd;	// the current value of the bottomStartPoint, after we decrement by y. This is done, so that
										// when we call setMountainBottomLayerContourPoints, the bottom layer is already known.
		if (x != 0)
		{
			currentRadius += in_expansionValue;
			currentNumberOfPoints += 4;
		}

		insertDoubleMRP(&triangleUpperStripMRPMap, x, mrpToAdd);		// add the MRP into the appropriate upper layer mountain strip.

		//std::cout << "!!! Preparing to add a contorued circuit, with start point at: "; in_startPoint.printPointCoords(); std::cout << std::endl;
		//int prepEnd = 3;
		//std::cin >> prepEnd;

		addContouredCircuit(&upperContourLineCircuits, &topCircuitCount, currentRadius, mrpToAdd.y, currentNumberOfPoints, in_startPoint);
	}
}

void CPV2Mountain::setMountainLowerLayersContourPoints(DoublePoint in_startPoint,
	int in_numberOfLayers,
	float in_distanceBetweenLayers,
	float in_startRadius,
	float in_expansionValue)
{
	int currentNumberOfPoints = 4;				// first layer always starts with 4 points (for now, this will probably change later)
	float currentRadius = in_startRadius;		// set the start radius
	DoublePoint bottomLayerMRP = triangleBottomStripMRPMap[0];
	for (int x = 0; x < in_numberOfLayers; x++)
	{
		if (x != 0)								// it isn't necessary to add on to the radius for the very first strip
		{
			currentRadius += in_expansionValue;
			currentNumberOfPoints += 4;
		}

		insertDoubleMRP(&triangleBottomStripMRPMap, x, bottomLayerMRP);
		addContouredCircuit(&bottomContourLineCircuits, &bottomCircuitCount, currentRadius, in_startPoint.y, currentNumberOfPoints, in_startPoint);
	}
}

void CPV2Mountain::setMRPsForBottomLayers()
{
	// if there's only one layer, the bottom MRP value is always the mountain's summit.
	if (numberOfLayers == 1)
	{
		triangleBottomStripMRPMap[0] = startPoint;
	}
	// ...otherwise, we need to get the next to last MRP from the top layer triangles, so use rbegin and increment it once, to store that value as the MRP that will be used for bottom layers.
	else
	{
		auto endMinusOne = triangleUpperStripMRPMap.rbegin();
		endMinusOne++;
		triangleBottomStripMRPMap[0] = endMinusOne->second;
	}

}

void CPV2Mountain::insertDoubleMRP(std::map<int, DoublePoint>* in_doubleMRPMapRef, 
									int in_stripValue, 
									DoublePoint in_doublePoint)
{
	(*in_doubleMRPMapRef)[in_stripValue] = in_doublePoint;
}

std::vector<ContouredTriangleV2*> CPV2Mountain::getProcessableContouredTriangles()
{
	std::vector<ContouredTriangleV2*> fTrianglePtrVector;

	// push upper strip ContouredTriangleV2 ptrs into vector
	for (auto& currentUpperStrip : typicalShellStrips)
	{
		for (auto& currentUpperTriangle : currentUpperStrip.second.triangleMap)
		{
			fTrianglePtrVector.push_back(&currentUpperTriangle.second);
		}
	}

	// push lower (mass-driver) strip ContouredTriangleV2 ptrs into vector
	for (auto& currentMassDriverStrip : shellMassDriverStrips)
	{
		for (auto& currentMassDriverTriangle : currentMassDriverStrip.second.triangleMap)
		{
			fTrianglePtrVector.push_back(&currentMassDriverTriangle.second);
		}
	}

	return fTrianglePtrVector;
}

void CPV2Mountain::addContouredCircuit(std::map<int, ContouredCircuit>* in_contouredCircuitMapRef,
									int* in_circuitCounterRef,
									float in_baseDistance,
									double in_contourElevation,
									int in_numberOfPoints,
									DoublePoint in_startPoint)
{
	ContouredCircuit currentCircuit(in_baseDistance, in_contourElevation, in_numberOfPoints, in_startPoint);
	(*in_contouredCircuitMapRef)[*in_circuitCounterRef] = currentCircuit;	// add the circuit; increment the line counter.
	(*in_circuitCounterRef)++;
}