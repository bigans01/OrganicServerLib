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
	// #############################################################################################################################
	// STEP 1 OVERVIEW: use all the values from each ForgedPolySet -- which represent the IDs of ECBPolys that this ContourPlan added -- for each blueprint, and 
	// then spawn the resulting EnclaveTriangles in the OREs (OrganicRawEnclaves) into the tempMap. When this is done, add the results to the containerMapMap.

	BPMassManagerV2 planMassManager(in_ecbMapRef, in_clientRef, this);
	planMassManager.buildContouredMass();		// Build the mass generated by the ContourPlan; this is a stand-alone, independent mass that exists in its own mass-space (separate from world mass-space)
	planMassManager.buildPersistentMasses();	// For each blueprint that the ContourPlan touches -- that also has an existing mass -- acquire a copy of the mass
												// from that blueprint before any changes are made in Phase 2.
	for (auto& currentForgedPolySetIter : allPolysRegistry.polySetRegistry)
	{
		// #############################################################################################################################
		// STEP 1.1): For the current blueprint, acquire the ECBPoly IDs of what the contour plan added to it, as well as ECBPoly IDs of what was already in the blueprint before this;
		// this will be needed for STEP 1.5, where we do a post-fracture check.
		EnclaveKeyDef::EnclaveKey blueprintKey = currentForgedPolySetIter.first;
		EnclaveCollectionBlueprint* currentServerBlueprintRef = in_ecbMapRef->getBlueprintRef(blueprintKey);
		ForgedPolySet originalSet = allPolysRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		OperableIntSet currentPlanAddedOrganicTriangles = originalSet.polySet;										// get the IDs of ECBPolys that this plan added to the current blueprint we're on
		OperableIntSet existingCurrentBlueprintPolyIDs = in_ecbMapRef->getBlueprintRef(blueprintKey)->produceECBPolyIDSet();
		existingCurrentBlueprintPolyIDs -= currentPlanAddedOrganicTriangles;

		// #############################################################################################################################
		// STEP 1.2): Produce the OREs for each OrganicTriangle that this ContourPlan added to this specific blueprint (produceRawEnclavesForPolySetWithTracking), 
		// AND get the OREs for each OrganicTriangle that already existed in the blueprint (via produceTrackedORESForOrganicTriangleIDs).
		//
		// Remember, If an existing OrganicTriangle in the blueprint has an ORE key that is also found in the tracked OREs of an OrganicTriangle that is produced by this plan, those
		// OrganicTriangles will have to be dissolved into smaller ones, which will be done by a call to planMassManager.scanForDissolvableTriangles(), and 
		// planMassManager.updatePersistentBlueprintPolys()
		//


		OrganicTriangleTracker* oreTrackerRef = planMassManager.getReformerTrackerRef(blueprintKey);	// remember,  keep track of each ORE that an individual OrganicTriangle touches (needed for SPoly post-fracture check), 
																										// for all ECBPolys in this blueprint
		// the tempMap has multiple purposes:
		// 1. Stores the EnclaveTriangles produced by an ORE that is generated by an OrganicTriangle from the ContourPlan;
		//	  the IDs of the OrganicTriangles are put into a referenced tracker (see call to produceRawEnclavesForPolySetWithTracking below)
		// 2. Stores the tracked OREs of existing OrganicTriangles that are persistent (i.e, OrganicTriangles not generated by the contour plan), and puts
		//	  them into a referenced tracker (see call to produceTrackedORESForOrganicTriangleIDs) below
		// 3. The produced EnclaveTriangles in each ORE of steps #1 and #2 are used in the call to spawnAndAppendEnclaveTriangleSkeletonsToBlueprint; at this point,
		//	  see the note S1 below

		// 1.) Get the tracked OREs of ECBPolys that WERE added by this contour plan
		ContouredPlanUtils::calculateTrackedOREsForAddedContourPolys(blueprintKey, currentServerBlueprintRef, currentPlanAddedOrganicTriangles.intSet, &in_ecbMapRef->blueprintMapMutex, oreTrackerRef);

		// 2.) Get the tracked OREs of ECBPolys that were NOT added by this contour plan
		ContouredPlanUtils::calculateTrackedOREsForAddedContourPolys(blueprintKey, currentServerBlueprintRef, existingCurrentBlueprintPolyIDs.intSet, &in_ecbMapRef->blueprintMapMutex, oreTrackerRef);

		// Use the ORE and EnclaveTriangle data that is stored in the planMassManager.contouredPlanMass, to apply collider operations with the selected collider class -- and/or similiar ones -- if need be, for this
		// blueprint that the ContourPlan affects/generated.
		//
		// NOTE S1: The call to appendContourPlanEnclaveTriangleSkeletons will also call the appropriate function based on whether or note the ORE exists:
		// --if the ORE doesn't exist, a new ORE is created, and it gets appended exactly once (so it's appended state becomes SINGLE_APPEND)
		// --if the ORE already exists, it's current appended state is iterated. (to MULTIPLE_APPEND).
		ContouredPlanUtils::appendContourPlanEnclaveTriangleSkeletons(blueprintKey, &planMassManager.contouredPlanMass, in_ecbMapRef, oreTrackerRef);
	}

	// Step 2) In this step, we must now apply the ContourPlans MassDriving to the persistent blueprints. Remember, the previous MassDriving operation was for 
	//		   the contoured mass of the planMassManager (see call to planMassManager.buildContouredMass() above). Mass driving had to be run for that mass, because
	//		   any Enclaves that are marked as "FULL" (having 100% of their mass occupied) can only be flagged through mass driving; additionally, any "empty" blocks 
	//		   are also filled as part of the mass driving. 

	//         For each blueprint that contained at least one SHELL_MASSDRIVER, take the original forged poly registry set for that blueprint, and
	//		   subtract the massDriverPolyRegistry set from it to produce a new set. This new set represents the OrganicTriangles that 
	//         will have to be traced in the blueprint that the mass driver begins in (but only for that initial blueprint)
	//
	//	NOTE:	remember, the shellMassDriverRegistry should already have been populated by the call to 
	//			the function, ContouredPlanV2Base::copyOverProducedECBPolys.

	//std::cout << "Printing unique blueprint keys from massDriverPolyRegistry: " << std::endl;
	for (auto& massDriverRegistryBegin : shellMassDriverRegistry.polySetRegistry)
	{
		// set up the set to use; grab the starting floor blueprint key
		EnclaveKeyDef::EnclaveKey startingFloorBlueprintKey = massDriverRegistryBegin.first;
		ForgedPolySet originalSet = allPolysRegistry.polySetRegistry[startingFloorBlueprintKey];	// get the original, unaltered set
		ForgedPolySet massDriverSet = massDriverRegistryBegin.second;				// the set that represents the polys identified as SHELL_MASSDRIVER type.
		ForgedPolySet startingFloorTerminatingSet = originalSet;
		for (auto& subtractionBegin : massDriverSet.polySet)
		{
			startingFloorTerminatingSet.polySet.erase(subtractionBegin);
		}

		std::cout << "!! Mass Driving blueprint Key: (" << massDriverRegistryBegin.first.x << ", " << massDriverRegistryBegin.first.y << ", " << massDriverRegistryBegin.first.z << ") " << std::endl;
		// now that the shell has been produced in all its affected blueprints, its now time to at last run the mass drivers.
		// a mass driver should pass:

		// 1.) a reference to the blueprint map
		// 2.) a reference to the ContourPlan's planPolyRegistry.polySetRegistry
		// 3.) the blueprint key of the blueprint that the mass driver starts in
		// 4.) the "newSet" that represents all polygons that a SHELL_MASSDRIVER polygon type may or may NOT hit
		// 5.) the "subtractingSet" that represents the SHELL_MASSDRIVER polygons
		//
		// The new function will need to produce a "phantom" blueprint that contains only the polygons in the originalSet, to make it seem
		// like those are the only ones that exist. To do this, the "originalSet" is used to trace 
		in_clientRef->OS->generateAndRunMassDriversForBlueprint(in_ecbMapRef, &allPolysRegistry.polySetRegistry, startingFloorBlueprintKey, startingFloorTerminatingSet, massDriverSet);

		//std::cout << "OrganicRawPolys updated... " << std::endl;

		std::cout << "::: Size of OREs in origin mass driving blueprint key: " << in_ecbMapRef->getFractureResultsMapRef(startingFloorBlueprintKey)->fractureResultsContainerMap.size() << std::endl;

	}

	// Step 3) Use the plan mass manager to apply updates, if there are any; 
	// the call to updatePersistentBlueprintPolys() will also update the
	// dependency state of any OREs that were produced as a result of shattering, to have a state of INDEPENDENT
	planMassManager.scanForDissolvableTriangles();	// attempts to "dissolve" contoured ecb polys into smaller ecb polys -- resulting in two categories of ECBPolys, "shattered" and "unshattered" -- and then compares them against persistent mass, 
													// and vice versa; in both cases, each smaller "shattered" ECBPoly takes up exactly 1 ORE. An "unshattered" ECBPoly can take up more than one ORE.
													// For both the "shattered" and "unshattered" types, each ORE key of an ECBPoly is checked to see if there is an already existing 
													// ORE that is FULL in the appropriate mass (appropriate mass meaning that the ECBPolys of the contour plan are compared against persistent mass, 
													// and that existing ECB Polys are compared against the contoured mass). If all ORE keys that an ECBPoly touches are FULL, that ECBPoly is erased.
													// In other words, if the single ORE key of a "shattered" ECBPoly is FULL, that is flagged for being erased; if all the ORE keys of an "unshattered" poly are FULL,
													// then that one is also flagged for erase. The erasing/inserting action will actually occur in the call to updatePersistentBlueprintPolys() below.

	planMassManager.updatePersistentBlueprintPolys();	// does the actual erasing/inserting of ECBPolys in the persistent/contour masses.

	planMassManager.updatedAffectedORESAsIndependent();

	/*

	Don't bother calling updateOREAsRMass, if using the ORELightweightCollider for collision operations; 

	// Step 4) Update any OREs that need to be set to RMass mode;
	//			The iterator will be for the keyed-found forged poly set, for the current blueprint we're looking at.
	//			The key of the iterator will be used as the blueprintKey to use.
	for (auto& planPolyRegistryBegin : allPolysRegistry.polySetRegistry)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin.first;					// get the key of the blueprint to check.
		EnclaveCollectionBlueprint* currentServerBlueprintRef = in_ecbMapRef->getBlueprintRef(blueprintKey);
		OrganicTriangleTracker* oreTrackerRef = planMassManager.getReformerTrackerRef(blueprintKey);				// remember,  keep track of each ORE that an individual OrganicTriangle touches (needed for SPoly post-fracture check), 
		for (auto& skeletonClearBegin : oreTrackerRef->modifiedORESet)
		{
			in_clientRef->OS->updateOREAsRMass(currentServerBlueprintRef, skeletonClearBegin);	// update the ORE, move over any pending solid blocks
		}
	}
	*/

	//std::cout << "### End of mass driver run. Time spent:" << organicelapsed.count() << std::endl;
	std::cout << "### ContouredMountain summit was:  "; startPoint.printPointCoords(); std::cout << std::endl;
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