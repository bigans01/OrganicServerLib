#include "stdafx.h"
#include "BlueprintMassManager.h"

void BlueprintMassManager::buildContouredMass()
{
	buildContourMassShell();					// first, build the shell.
	produceOREsForShellPolys();					// build the OREs for each ECBPoly of the shell
	runMassDriversForIndependentMass();			// generate the mass.
}

void BlueprintMassManager::buildContourMassShell()
{
	auto blueprintsWithShellsToTransferBegin = contourPlanRef->adherenceData.adherenceOrder.begin();
	auto blueprintsWithShellsToTransferEnd = contourPlanRef->adherenceData.adherenceOrder.end();
	for (; blueprintsWithShellsToTransferBegin != blueprintsWithShellsToTransferEnd; blueprintsWithShellsToTransferBegin++)
	{
		// find the forged poly set, for the current blueprint we're looking at
		auto refedContourPlanPolySetRegistryBegin = contourPlanRef->planPolyRegistry.polySetRegistry.find(*blueprintsWithShellsToTransferBegin);
		EnclaveKeyDef::EnclaveKey currentFirstPassBlueprintKey = refedContourPlanPolySetRegistryBegin->first;		// get the key of the blueprint to check.
		OperableIntSet firstMassPassContourAddedTriangles = refedContourPlanPolySetRegistryBegin->second.polySet;		// get an OperableIntSet of the OrganicTriangles that the ref'ed ContourPlan
																														// added to the blueprint.
		copyMassShellPolysFromServerToMass(currentFirstPassBlueprintKey, firstMassPassContourAddedTriangles);	// transfer the specific OrganicTriangles into this mass
	}
}

void BlueprintMassManager::copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet)
{
	EnclaveCollectionBlueprint* currentBlueprintRef = &(*serverBlueprintsRef)[in_blueprintKey];
	contouredPlanShellBlueprintKeyAndSetPairs[in_blueprintKey] = in_contourAddedOrganicTrianglesSet;
	std::map<int, ECBPoly>* currentPolyMapRef = &currentBlueprintRef->primaryPolygonMap;
	auto setBegin = in_contourAddedOrganicTrianglesSet.intSet.begin();
	auto setEnd = in_contourAddedOrganicTrianglesSet.intSet.end();
	for (; setBegin != setEnd; setBegin++)
	{
		ECBPoly* polyRef = &(*currentPolyMapRef).find(*setBegin)->second;	// grab a ref from the persistent state of the blueprint on the server
		contouredPlanEntireShellRegistry.addToPolyset(in_blueprintKey, *setBegin);// add to the entire shell
		if (polyRef->polyType == ECBPolyType::SHELL_MASSDRIVER)					// if it's a shell mass driver, insert it into the contouredPlanMassDriverRegistry
		{
			contouredPlanMassDriverRegistry.addToPolyset(in_blueprintKey, *setBegin);
		}

		// create the blueprint in massA, if it doesn't exist (it shouldn't, but the check is for safety)
		contouredPlanMass.insertBlueprintIfNonExistent(in_blueprintKey);					
		EnclaveCollectionBlueprint* contouredPlanMassBlueprintRef = contouredPlanMass.getBlueprintRef(in_blueprintKey);
		currentBlueprintRef->copyExistingECBPolysToOtherBlueprint(in_contourAddedOrganicTrianglesSet, contouredPlanMassBlueprintRef);

	}
}

void BlueprintMassManager::produceOREsForShellPolys()
{
	auto contouredPlanShellBegin = contouredPlanShellBlueprintKeyAndSetPairs.begin();
	auto contouredPlanShellEnd = contouredPlanShellBlueprintKeyAndSetPairs.end();
	for (; contouredPlanShellBegin != contouredPlanShellEnd; contouredPlanShellBegin++)
	{
		organicClientRef->OS->produceRawEnclavesForPolySet(contouredPlanMass.getFractureResultsMapRef(contouredPlanShellBegin->first),
														contouredPlanShellBegin->first,
														contouredPlanMass.getBlueprintRef(contouredPlanShellBegin->first),
														contouredPlanShellBegin->second.intSet);
	}
}

void BlueprintMassManager::runMassDriversForIndependentMass()
{
	auto massDriverRegistryBegin = contouredPlanMassDriverRegistry.polySetRegistry.begin();
	auto massDriverRegistryEnd = contouredPlanMassDriverRegistry.polySetRegistry.end();
	for (; massDriverRegistryBegin != massDriverRegistryEnd; massDriverRegistryBegin++)
	{
		// set up the set to use
		EnclaveKeyDef::EnclaveKey blueprintKey = massDriverRegistryBegin->first;
		ForgedPolySet originalSet = contouredPlanEntireShellRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set; for the entire outer shell
		//ForgedPolySet subtractingSet = massDriverRegistryBegin->second;
		ForgedPolySet massDriverSet = massDriverRegistryBegin->second;				// the set that represents the polys identified as SHELL_MASSDRIVER type.
		ForgedPolySet startingFloorTerminatingSet = originalSet;

		auto subtractionBegin = massDriverSet.polySet.begin();
		auto subtractionEnd = massDriverSet.polySet.end();
		for (subtractionBegin; subtractionBegin != subtractionEnd; subtractionBegin++)
		{
			startingFloorTerminatingSet.polySet.erase(*subtractionBegin);
		}

		organicClientRef->OS->generateAndRunMassDriversForBlueprint(contouredPlanMass.getBlueprintMapRef(),
																&contouredPlanEntireShellRegistry.polySetRegistry,
																blueprintKey,
																startingFloorTerminatingSet,
																massDriverSet);
	}
}

OrganicTriangleTracker* BlueprintMassManager::getReformerTrackerRef(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	return &reformerTracker[in_blueprintKey].reformerTracker;
}

void BlueprintMassManager::buildPersistentMasses()
{
	auto contouredMassMapRef = contouredPlanMass.getBlueprintMapRef();
	auto contouredBlueprintsBegin = contouredMassMapRef->begin();
	auto contouredBlueprintsEnd = contouredMassMapRef->end();
	for (; contouredBlueprintsBegin != contouredBlueprintsEnd; contouredBlueprintsBegin++)
	{
		OperableIntSet contouredPlanAddedTrianglesToCurrentBlueprint = contourPlanRef->planPolyRegistry.polySetRegistry[contouredBlueprintsBegin->first].polySet;
		OperableIntSet persistentBlueprintAllTriangles = (*serverBlueprintsRef)[contouredBlueprintsBegin->first].produceECBPolyIDSet();

		std::cout << "Checking for existing persistent mass in key (" << contouredBlueprintsBegin->first.x << ", "  
																	  << contouredBlueprintsBegin->first.y << ", " 
																	  << contouredBlueprintsBegin->first.z << ") " << std::endl;

		std::cout << "! (PRE-ADD) Contoured poly ID set size: " << contouredPlanAddedTrianglesToCurrentBlueprint.intSet.size() << std::endl;
		std::cout << "! (PRE-ADD) Persistent poly ID set size: " << persistentBlueprintAllTriangles.intSet.size() << std::endl;

		persistentBlueprintAllTriangles -= contouredPlanAddedTrianglesToCurrentBlueprint;

		// CASE 1: for REFORM_BY_COLLISION
		if 
		(
			(!contouredPlanAddedTrianglesToCurrentBlueprint.intSet.empty())
			&&
			(!persistentBlueprintAllTriangles.intSet.empty())
		)
		{
			std::cout << "Notice! persistent addition will be required into the persistentMass, as a REFORM_BY_COLLISION Key:(" << contouredBlueprintsBegin->first.x << ", "
				<< contouredBlueprintsBegin->first.y << ", "
				<< contouredBlueprintsBegin->first.z << ") " << std::endl;

			std::cout << "! Contoured poly ID set size: " << contouredPlanAddedTrianglesToCurrentBlueprint.intSet.size() << std::endl;
			std::cout << "! Persistent poly ID set size: " << persistentBlueprintAllTriangles.intSet.size() << std::endl;

			EnclaveCollectionBlueprint* blueprintFractureCopyDestinationRef = persistentMass.getBlueprintRef(contouredBlueprintsBegin->first);
			EnclaveCollectionBlueprint* blueprintFractureCopySourceRef = &(*serverBlueprintsRef)[contouredBlueprintsBegin->first];
			blueprintFractureCopySourceRef->copyFractureResultsMapToOtherBlueprint(blueprintFractureCopyDestinationRef);

			ECBPolyReformer collisionReformer(ECBPolyReformerType::REFORM_BY_COLLISION,
											contouredPlanAddedTrianglesToCurrentBlueprint,
											contouredPlanMass.getFractureResultsMapRef(contouredBlueprintsBegin->first),
											persistentBlueprintAllTriangles,
											persistentMass.getFractureResultsMapRef(contouredBlueprintsBegin->first)
										);
			reformerTracker[contouredBlueprintsBegin->first] = collisionReformer;

			// let's get counts of what we just copied.
			/*
			auto copiedOREsBegin = blueprintFractureCopyDestinationRef->fractureResults.fractureResultsContainerMap.begin();
			auto copiedOREsEnd = blueprintFractureCopyDestinationRef->fractureResults.fractureResultsContainerMap.end();
			int allCounter = 0;
			int fullCounter = 0;
			int lodCounter = 0;
			for (; copiedOREsBegin != copiedOREsEnd; copiedOREsBegin++)
			{
				if (copiedOREsBegin->second.getLodState() == ORELodState::FULL)
				{
					fullCounter++;
				}
				else if (copiedOREsBegin->second.getLodState() == ORELodState::LOD_ENCLAVE)
				{
					lodCounter++;
				}
				allCounter++;
			}

			std::cout << "--- copied fracture results map stats are: " << std::endl;
			std::cout << ":: Number of OREs: " << allCounter << std::endl;
			std::cout << ":: Number of FULL OREs: " << fullCounter << std::endl;
			std::cout << ":: Number of LOD_ENCLAVE OREs: " << lodCounter << std::endl;
			*/
		}

		// CASE 2: for REFORM_BY_OVERWRITE
		// 
		// The case for an overwrite is: the ContouredPlan didn't add any ECBPolys to the blueprint, but the MassDriver run for the persistent mass did, --AND--
		// that same blueprint also has existing, persistent ECBPolys that would exist prior to the ContourPlan adding this mass. Therefore, the ContourPlan's mass
		// for this blueprint fills the blueprint completely, meaning no persistent ECBPolys should exist. Therefore: 
		// 
		// --the ContourPlan adds no ECBPolys to this blueprint
		// AND
		// --there are existing (persistent) ECBPolys in the blueprint.
		else if 
		(
			(contouredPlanAddedTrianglesToCurrentBlueprint.intSet.empty())
			&&
			(!persistentBlueprintAllTriangles.intSet.empty())
		)
		{
			std::cout << "Notice! persistent addition will be required into the persistentMass, as a REFORM_BY_OVERWRITE Key:(" << contouredBlueprintsBegin->first.x << ", "
				<< contouredBlueprintsBegin->first.y << ", "
				<< contouredBlueprintsBegin->first.z << ") " << std::endl;
			EnclaveCollectionBlueprint* blueprintFractureCopyDestinationRef = persistentMass.getBlueprintRef(contouredBlueprintsBegin->first);
			EnclaveCollectionBlueprint* blueprintFractureCopySourceRef = &(*serverBlueprintsRef)[contouredBlueprintsBegin->first];
			blueprintFractureCopySourceRef->copyFractureResultsMapToOtherBlueprint(blueprintFractureCopyDestinationRef);

			ECBPolyReformer collisionReformer(ECBPolyReformerType::REFORM_BY_OVERWRITE,
				contouredPlanAddedTrianglesToCurrentBlueprint,
				contouredPlanMass.getFractureResultsMapRef(contouredBlueprintsBegin->first),
				persistentBlueprintAllTriangles,
				persistentMass.getFractureResultsMapRef(contouredBlueprintsBegin->first)
			);
			reformerTracker[contouredBlueprintsBegin->first] = collisionReformer;

			// let's get counts of what we just copied.
			/*
			auto copiedOREsBegin = blueprintFractureCopyDestinationRef->fractureResults.fractureResultsContainerMap.begin();
			auto copiedOREsEnd = blueprintFractureCopyDestinationRef->fractureResults.fractureResultsContainerMap.end();
			int allCounter = 0;
			int fullCounter = 0;
			int lodCounter = 0;
			for (; copiedOREsBegin != copiedOREsEnd; copiedOREsBegin++)
			{
				if (copiedOREsBegin->second.getLodState() == ORELodState::FULL)
				{
					fullCounter++;
				}
				else if (copiedOREsBegin->second.getLodState() == ORELodState::LOD_ENCLAVE)
				{
					lodCounter++;
				}
				allCounter++;
			}

			std::cout << "--- copied fracture results map stats are: " << std::endl;
			std::cout << ":: Number of OREs: " << allCounter << std::endl;
			std::cout << ":: Number of FULL OREs: " << fullCounter << std::endl;
			std::cout << ":: Number of LOD_ENCLAVE OREs: " << lodCounter << std::endl;
			*/
		}
	}

	int waitVal = 3;
	std::cin >> waitVal;
}

void BlueprintMassManager::scanForDissolvableTriangles()
{
	// Step 1: cycle through each ORE tracker, and call determineOrganicTrianglesToDissolve().
	auto dissolveCallsBegin = reformerTracker.begin();
	auto dissolveCallsEnd = reformerTracker.end();
	for (; dissolveCallsBegin != dissolveCallsEnd; dissolveCallsBegin++)
	{
		dissolveCallsBegin->second.reformerTracker.determineOrganicTrianglesToDissolve();
	}

	// Step 2: run analysis.

	auto ecbPolyReformerTrackerBegin = reformerTracker.begin();
	auto ecbPolyReformerTrackerEnd = reformerTracker.end();
	for (; ecbPolyReformerTrackerBegin != ecbPolyReformerTrackerEnd; ecbPolyReformerTrackerBegin++)
	{
		// only bother getting an instance of an ECBPolyReformer if it is set to have a ECBPolyReformerType of REFORM_BY_OVERWRITE or REFORM_BY_COLLISION
		if
		(
			(ecbPolyReformerTrackerBegin->second.reformerType == ECBPolyReformerType::REFORM_BY_COLLISION)
			||
			(ecbPolyReformerTrackerBegin->second.reformerType == ECBPolyReformerType::REFORM_BY_OVERWRITE)
		)
		{
			std::cout << "....Analyzing reformer for blueprint key (" << ecbPolyReformerTrackerBegin->first.x << ", " << ecbPolyReformerTrackerBegin->first.y << ", " << ecbPolyReformerTrackerBegin->first.z << ") " << std::endl;
			std::cout << "..Contoured ECBPoly stats are: " << std::endl;

			// need to get the ID that would be used for the next ECBPoly; this value would be from the persistent blueprint. 
			int nextECBPolyIDForCurrentBlueprint = (*serverBlueprintsRef)[ecbPolyReformerTrackerBegin->first].fetchNextECBPolyKeyID();

			/*
			std::cout << ". Old set size: " << ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.size() << std::endl;
			std::cout << ". New set size: " << ecbPolyReformerTrackerBegin->second.persistentMassShellECBPolyIDs.intSet.size() << std::endl;
			std::cout << ". Next ECBPoly key ID: " << nextECBPolyIDForCurrentBlueprint << std::endl;

			std::cout << ". Size of modified ore set: " << ecbPolyReformerTrackerBegin->second.reformerTracker.modifiedORESet.size() << std::endl;
			std::cout << ". Size of dissolved erased: " << ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleErasedOREs.size() << std::endl;
			std::cout << ". Size of dissolved remaining: " << ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleRemainingOREs.size() << std::endl;
			
			// First pass: build new ECBPolys from dissolved ECBPolys, for the ECBPolys that are in the contour plan.
			auto contouredSetBegin = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.begin();
			auto contouredSetEnd = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.end();
			for (; contouredSetBegin != contouredSetEnd; contouredSetBegin++)
			{
				auto wasCurrentECBPolyIDErased = ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleErasedOREs.find(*contouredSetBegin);	// attempt to find an entry indicating some OREs were erased was found.
				if (wasCurrentECBPolyIDErased != ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleErasedOREs.end())	// it was found
				{
					std::cout << ".Contoured Set ECBPoly with ID " << *contouredSetBegin << " was dissolved, stats are: " << std::endl;
					std::cout << ".Erased Ore keys: " << std::endl;
					auto erasedKeysBegin = wasCurrentECBPolyIDErased->second.begin();
					auto erasedKeysEnd = wasCurrentECBPolyIDErased->second.end();
					for (; erasedKeysBegin != erasedKeysEnd; erasedKeysBegin++)
					{
						std::cout << "(" << erasedKeysBegin->x << ", " << erasedKeysBegin->y << ", " << erasedKeysBegin->z << ") " << std::endl;
					}

					std::cout << ".Remaining Ore keys: " << std::endl;
					auto remainingOresBegin = ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleRemainingOREs[*contouredSetBegin].begin();
					auto remainingOresEnd = ecbPolyReformerTrackerBegin->second.reformerTracker.dissolvedTriangleRemainingOREs[*contouredSetBegin].end();
					for (; remainingOresBegin != remainingOresEnd; remainingOresBegin++)
					{
						std::cout << "(" << remainingOresBegin->x << ", " << remainingOresBegin->y << ", " << remainingOresBegin->z << ") " << std::endl;
					}

					int dissolvedWait = 3;
					std::cin >> dissolvedWait;
				}
			}
			*/
			ecbPolyReformerTrackerBegin->second.produceFirstPassECBPolys(&nextECBPolyIDForCurrentBlueprint);
		}
	}
}