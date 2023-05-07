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
	/*
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
	*/

	for (auto& currentRegistry : contourPlanRef->planPolyRegistry.polySetRegistry)
	{
		EnclaveKeyDef::EnclaveKey currentFirstPassBlueprintKey = currentRegistry.first;
		OperableIntSet firstMassPassContourAddedTriangles = currentRegistry.second.polySet;
		copyMassShellPolysFromServerToMass(currentFirstPassBlueprintKey, firstMassPassContourAddedTriangles);
	}
}

void BlueprintMassManager::checkStateOfSpecificOre(EnclaveKeyDef::EnclaveKey in_blueprintKeyToCheck,
	EnclaveKeyDef::EnclaveKey in_oreKeyToCheck)
{
	std::cout << "(BlueprintMassManager::checkStateOfSpecificOre): Checking the status of the ORE at: Blueprint ";
	in_blueprintKeyToCheck.printKey();
	std::cout << " | ORE: ";
	in_oreKeyToCheck.printKey();
	std::cout << std::endl;

	// Check the persistent mass.
	std::cout << "(BlueprintMassManager::checkStateOfSpecificOre): -> checking for the ORE in PERSISTENT mass. " << std::endl;
	bool persistentBlueprintFinder = persistentMass.checkIfBlueprintExists(in_blueprintKeyToCheck);
	if (persistentBlueprintFinder == true)
	{
		std::cout << "(BlueprintMassManager::checkStateOfSpecificOre): -> Found Blueprint of the ORE. " << std::endl;
		auto checkIfOREIsInPersistentMass = persistentMass.checkIfBlueprintContainsSpecificOre(in_blueprintKeyToCheck, in_oreKeyToCheck);
		if (checkIfOREIsInPersistentMass == true)
		{
			std::cout << "!!! Found ORE in persistent mass. " << std::endl;
			ORELodState currentLodState = persistentMass.getOrganicRawEnclaveRef(in_blueprintKeyToCheck, in_oreKeyToCheck)->getLodState();
			std::string lodStatePrefix = "(BlueprintMassManager::checkStateOfSpecificOre): LOD state of ORE (PERSISTENT) is: ";
			switch (currentLodState)
			{
				case ORELodState::LOD_ENCLAVE_SMATTER: { lodStatePrefix += "LOD_ENCLAVE_SMATTER"; break; }
				case ORELodState::LOD_ENCLAVE_RMATTER: { lodStatePrefix += "LOD_ENCLAVE_RMATTER"; break; }
				case ORELodState::LOD_BLOCK: { lodStatePrefix += "LOD_BLOCK"; break; }
				case ORELodState::FULL: { lodStatePrefix += "FULL"; break; }
			}
			std::cout << lodStatePrefix << std::endl;

		}
		else
		{
			std::cout << "Blueprint exists in PERSISTENT mass, but did not find ORE. " << std::endl;
		}
	}
	else
	{
		std::cout << "Did not find Blueprint for the ORE in PERSISTENT mass." << std::endl;
	}

	// Check the contoured mass.
	std::cout << "(BlueprintMassManager::checkStateOfSpecificOre): -> checking for the ORE in CONTOURED mass. " << std::endl;
	bool contouredBlueprintFinder = contouredPlanMass.checkIfBlueprintExists(in_blueprintKeyToCheck);
	if (contouredBlueprintFinder == true)
	{
		std::cout << "(BlueprintMassManager::checkStateOfSpecificOre): -> Found Blueprint of the ORE. " << std::endl;
		auto checkIfOREIsInContouredMass = contouredPlanMass.checkIfBlueprintContainsSpecificOre(in_blueprintKeyToCheck, in_oreKeyToCheck);
		if (checkIfOREIsInContouredMass == true)
		{
			std::cout << "!!! Found ORE in persistent mass. " << std::endl;
			ORELodState currentLodState = contouredPlanMass.getOrganicRawEnclaveRef(in_blueprintKeyToCheck, in_oreKeyToCheck)->getLodState();
			std::string lodStatePrefix = "(BlueprintMassManager::checkStateOfSpecificOre): LOD state of ORE (CONTOURED) is: ";
			switch (currentLodState)
			{
				case ORELodState::LOD_ENCLAVE_SMATTER: { lodStatePrefix += "LOD_ENCLAVE_SMATTER"; break; }
				case ORELodState::LOD_ENCLAVE_RMATTER: { lodStatePrefix += "LOD_ENCLAVE_RMATTER"; break; }
				case ORELodState::LOD_BLOCK: { lodStatePrefix += "LOD_BLOCK"; break; }
				case ORELodState::FULL: { lodStatePrefix += "FULL"; break; }
			}
			std::cout << lodStatePrefix << std::endl;

		}
		else
		{
			std::cout << "Blueprint exists in CONTOURED mass, but did not find ORE. " << std::endl;
		}
	}
	else
	{
		std::cout << "Did not find Blueprint for the ORE in CONTOURED mass." << std::endl;
	}

	std::cout << "Done with call of (BlueprintMassManager::checkStateOfSpecificOre); enter key to continue." << std::endl;
	int doneSearch = 3;
	std::cin >> doneSearch;
}

void BlueprintMassManager::copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet)
{
	EnclaveCollectionBlueprint* currentBlueprintRef = managerEcbMapRef->getBlueprintRef(in_blueprintKey);
	contouredPlanShellBlueprintKeyAndSetPairs[in_blueprintKey] = in_contourAddedOrganicTrianglesSet;
	auto setBegin = in_contourAddedOrganicTrianglesSet.intSet.begin();
	auto setEnd = in_contourAddedOrganicTrianglesSet.intSet.end();
	for (; setBegin != setEnd; setBegin++)
	{
		ECBPoly* polyRef = &currentBlueprintRef->getPolyIterFromMap(*setBegin)->second;	// grab a ref from the persistent state of the blueprint on the server
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

		// for the ORE's we just spawned, make sure to load their skeleton containers.
		auto spawnedOREsBegin = contouredPlanMass.getFractureResultsMapRef(contouredPlanShellBegin->first)->fractureResultsContainerMap.begin();
		auto spawnedOREsEnd = contouredPlanMass.getFractureResultsMapRef(contouredPlanShellBegin->first)->fractureResultsContainerMap.end();
		for (; spawnedOREsBegin != spawnedOREsEnd; spawnedOREsBegin++)
		{
			spawnedOREsBegin->second.loadSkeletonContainersFromEnclaveContainers();
		}
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

		
		organicClientRef->OS->generateAndRunMassDriversForBlueprint(&contouredPlanMass,
			&contouredPlanEntireShellRegistry.polySetRegistry,
			blueprintKey,
			startingFloorTerminatingSet,
			massDriverSet);
		
	}
}

OrganicTriangleTracker* BlueprintMassManager::getReformerTrackerRef(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	return &reformerTracker[in_blueprintKey].triangleTracker;
}

void BlueprintMassManager::buildPersistentMasses()
{
	auto contouredBlueprintsBegin = contouredPlanMass.getBlueprintBeginIter();
	auto contouredBlueprintsEnd = contouredPlanMass.getBlueprintEndIter();

	for (; contouredBlueprintsBegin != contouredBlueprintsEnd; contouredBlueprintsBegin++)
	{
		OperableIntSet contouredPlanAddedTrianglesToCurrentBlueprint = contourPlanRef->planPolyRegistry.polySetRegistry[contouredBlueprintsBegin->first].polySet;
		OperableIntSet persistentBlueprintAllTriangles = managerEcbMapRef->getBlueprintRef(contouredBlueprintsBegin->first)->produceECBPolyIDSet();

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
			EnclaveCollectionBlueprint* blueprintFractureCopySourceRef = managerEcbMapRef->getBlueprintRef(contouredBlueprintsBegin->first);
			blueprintFractureCopySourceRef->copyFractureResultsMapToOtherBlueprint(blueprintFractureCopyDestinationRef);

			ECBPolyReformer collisionReformer(contouredBlueprintsBegin->first,
												managerEcbMapRef->getBlueprintRef(contouredBlueprintsBegin->first),
												ECBPolyReformerType::REFORM_BY_COLLISION,
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
				else if (copiedOREsBegin->second.getLodState() == ORELodState::LOD_ENCLAVE_SMATTER)
				{
					lodCounter++;
				}
				allCounter++;
			}

			std::cout << "--- copied fracture results map stats are: " << std::endl;
			std::cout << ":: Number of OREs: " << allCounter << std::endl;
			std::cout << ":: Number of FULL OREs: " << fullCounter << std::endl;
			std::cout << ":: Number of LOD_ENCLAVE_SMATTER OREs: " << lodCounter << std::endl;
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
			EnclaveCollectionBlueprint* blueprintFractureCopySourceRef = managerEcbMapRef->getBlueprintRef(contouredBlueprintsBegin->first);
			blueprintFractureCopySourceRef->copyFractureResultsMapToOtherBlueprint(blueprintFractureCopyDestinationRef);

			ECBPolyReformer collisionReformer(contouredBlueprintsBegin->first,
												managerEcbMapRef->getBlueprintRef(contouredBlueprintsBegin->first),
												ECBPolyReformerType::REFORM_BY_OVERWRITE,
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
				else if (copiedOREsBegin->second.getLodState() == ORELodState::LOD_ENCLAVE_SMATTER)
				{
					lodCounter++;
				}
				allCounter++;
			}

			std::cout << "--- copied fracture results map stats are: " << std::endl;
			std::cout << ":: Number of OREs: " << allCounter << std::endl;
			std::cout << ":: Number of FULL OREs: " << fullCounter << std::endl;
			std::cout << ":: Number of LOD_ENCLAVE_SMATTER OREs: " << lodCounter << std::endl;
			*/
		}
	}

	//int waitVal = 3;
	//std::cin >> waitVal;
}

void BlueprintMassManager::scanForDissolvableTriangles()
{
	// Step 1: cycle through each ORE tracker, and call determineOrganicTrianglesToDissolve();
	// in other words, if two triangles share an ORE, they are both dissolvable.
	auto dissolveCallsBegin = reformerTracker.begin();
	auto dissolveCallsEnd = reformerTracker.end();
	for (; dissolveCallsBegin != dissolveCallsEnd; dissolveCallsBegin++)
	{
		dissolveCallsBegin->second.triangleTracker.determineOrganicTrianglesToDissolve();
	}

	// Step 2: run analysis for each blueprint

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
			int nextECBPolyIDForCurrentBlueprint = managerEcbMapRef->getBlueprintRef(ecbPolyReformerTrackerBegin->first)->fetchNextECBPolyKeyID();

			/*
			std::cout << ". Old set size: " << ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.size() << std::endl;
			std::cout << ". New set size: " << ecbPolyReformerTrackerBegin->second.persistentMassShellECBPolyIDs.intSet.size() << std::endl;
			std::cout << ". Next ECBPoly key ID: " << nextECBPolyIDForCurrentBlueprint << std::endl;

			std::cout << ". Size of modified ore set: " << ecbPolyReformerTrackerBegin->second.reformerTracker.modifiedORESet.size() << std::endl;
			std::cout << ". Size of dissolved erased: " << ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleErasedOREs.size() << std::endl;
			std::cout << ". Size of dissolved remaining: " << ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleRemainingOREs.size() << std::endl;
			
			// First pass: build new ECBPolys from dissolved ECBPolys, for the ECBPolys that are in the contour plan.
			auto contouredSetBegin = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.begin();
			auto contouredSetEnd = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.end();
			for (; contouredSetBegin != contouredSetEnd; contouredSetBegin++)
			{
				auto wasCurrentECBPolyIDErased = ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleErasedOREs.find(*contouredSetBegin);	// attempt to find an entry indicating some OREs were erased was found.
				if (wasCurrentECBPolyIDErased != ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleErasedOREs.end())	// it was found
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
					auto remainingOresBegin = ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleRemainingOREs[*contouredSetBegin].begin();
					auto remainingOresEnd = ecbPolyReformerTrackerBegin->second.reformerTracker.shatteredTriangleRemainingOREs[*contouredSetBegin].end();
					for (; remainingOresBegin != remainingOresEnd; remainingOresBegin++)
					{
						std::cout << "(" << remainingOresBegin->x << ", " << remainingOresBegin->y << ", " << remainingOresBegin->z << ") " << std::endl;
					}

					int dissolvedWait = 3;
					std::cin >> dissolvedWait;
				}
			}
			*/
			ecbPolyReformerTrackerBegin->second.processContouredPolysAgainstPersistentMass(&nextECBPolyIDForCurrentBlueprint);
			ecbPolyReformerTrackerBegin->second.processPersistentPolysAgainstContouredMass(&nextECBPolyIDForCurrentBlueprint);
		}
	}
}

void BlueprintMassManager::updatePersistentBlueprintPolys()
{

	// First pass: contoured ECBPolys.

	// For each blueprint that the contour plan touched, update that blueprint 
	auto ecbPolyReformerTrackerBegin = reformerTracker.begin();
	auto ecbPolyReformerTrackerEnd = reformerTracker.end();
	for (; ecbPolyReformerTrackerBegin != ecbPolyReformerTrackerEnd; ecbPolyReformerTrackerBegin++)
	{
		/*
		std::cout << ">>>>>>>>>> Checking current ecbPolyReformer, for blueprint key: (" << ecbPolyReformerTrackerBegin->first.x << ", " << ecbPolyReformerTrackerBegin->first.y << ", " << ecbPolyReformerTrackerBegin->first.z << ") " << std::endl;
		std::cout << ">>>>>>>>>> Size of contouredMassShellECBPolyIDs is: " << ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.size() << std::endl;
		std::cout << ">>>>>>>>>> Size of firstPassResultingShatteredECBPolys is: " << ecbPolyReformerTrackerBegin->second.firstPassResultingShatteredECBPolys.size() << std::endl;
		int firstPassWait = 3;
		std::cin >> firstPassWait;
		*/

		auto currentReformerContouredSetBegin = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.begin();
		auto currentReformerContouredSetEnd = ecbPolyReformerTrackerBegin->second.contouredMassShellECBPolyIDs.intSet.end();
		for (; currentReformerContouredSetBegin != currentReformerContouredSetEnd; currentReformerContouredSetBegin++)
		{
			// first check: check for any shattered contoured ECBPolys that were spawned for the original contoured ECBPoly ID.
			auto wasPolyShattered = ecbPolyReformerTrackerBegin->second.firstPassShatteredPolysSet.find(*currentReformerContouredSetBegin);
			if (wasPolyShattered != ecbPolyReformerTrackerBegin->second.firstPassShatteredPolysSet.end())
			{
				auto currentShatteredPolyGroupBegin = ecbPolyReformerTrackerBegin->second.firstPassResultingShatteredECBPolys.begin();
				auto currentShatteredPolyGroupEnd = ecbPolyReformerTrackerBegin->second.firstPassResultingShatteredECBPolys.end();
				for (; currentShatteredPolyGroupBegin != currentShatteredPolyGroupEnd; currentShatteredPolyGroupBegin++)
				{
					for (auto& currentPolyInGroup : currentShatteredPolyGroupBegin->second.shatteredEcbPolyMap)
					{
						ecbPolyReformerTrackerBegin->second.serverBlueprintRef->insertPolyWithKeyValue(currentPolyInGroup.first, currentPolyInGroup.second);
					}
				}

				// remove the ECBPoly that these contoured (shattered) ECBPoly(s) were spawned from, in the persistent map.
				ecbPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(*wasPolyShattered);
			}
		}

		// second check: check for any contoured ECBPolys that weren't shattered, but need to be erased.
		auto currentErasePolyCheckBegin = ecbPolyReformerTrackerBegin->second.firstPassUnshatteredECBPolysToErase.begin();
		auto currentErasePolyCheckEnd = ecbPolyReformerTrackerBegin->second.firstPassUnshatteredECBPolysToErase.end();
		for (; currentErasePolyCheckBegin != currentErasePolyCheckEnd; currentErasePolyCheckBegin++)
		{
			ecbPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(*currentErasePolyCheckBegin);
		}

	}

	// Second pass: persistent ECBPolys.

	auto secondPassECBPolyReformerTrackerBegin = reformerTracker.begin();
	auto secondPassECBPolyReformerTrackerEnd = reformerTracker.end();
	for (; secondPassECBPolyReformerTrackerBegin != secondPassECBPolyReformerTrackerEnd; secondPassECBPolyReformerTrackerBegin++)
	{

		auto currentReformerPersistentSetBegin = secondPassECBPolyReformerTrackerBegin->second.persistentMassShellECBPolyIDs.intSet.begin();
		auto currentReformerPersistentSetEnd = secondPassECBPolyReformerTrackerBegin->second.persistentMassShellECBPolyIDs.intSet.end();
		for (; currentReformerPersistentSetBegin != currentReformerPersistentSetEnd; currentReformerPersistentSetBegin++)
		{
			// first check: check for any shattered persistent ECBPolys that were spawned for the original contoured ECBPoly ID.
			auto wasPolyShattered = secondPassECBPolyReformerTrackerBegin->second.secondPassShatteredPolysSet.find(*currentReformerPersistentSetBegin);
			if (wasPolyShattered != secondPassECBPolyReformerTrackerBegin->second.secondPassShatteredPolysSet.end())
			{
				auto currentShatteredPolyGroupBegin = secondPassECBPolyReformerTrackerBegin->second.secondPassResultingShatteredECBPolys.begin();
				auto currentShatteredPolyGroupEnd = secondPassECBPolyReformerTrackerBegin->second.secondPassResultingShatteredECBPolys.end();
				for (; currentShatteredPolyGroupBegin != currentShatteredPolyGroupEnd; currentShatteredPolyGroupBegin++)
				{
					for (auto& currentPolyInGroup : currentShatteredPolyGroupBegin->second.shatteredEcbPolyMap)
					{
						secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->insertPolyWithKeyValue(currentPolyInGroup.first, currentPolyInGroup.second);
					}
				}

				// remove the ECBPoly that these contoured (shattered) ECBPoly(s) were spawned from, in the persistent map.
				secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(*wasPolyShattered);


			}

		}

		// second check: check for any persistent ECBPolys that weren't shattered, but need to be erased.	
		auto currentPersistentErasePolyCheckBegin = secondPassECBPolyReformerTrackerBegin->second.secondPassUnshatteredECBPolysToErase.begin();
		auto currentPersistentErasePolyCheckEnd = secondPassECBPolyReformerTrackerBegin->second.secondPassUnshatteredECBPolysToErase.end();
		for (; currentPersistentErasePolyCheckBegin != currentPersistentErasePolyCheckEnd; currentPersistentErasePolyCheckBegin++)
		{
			secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(*currentPersistentErasePolyCheckBegin);
		}
	}
}