#include "stdafx.h"
#include "BPMassManagerV2.h"

void BPMassManagerV2::buildContouredMass()
{
	buildContourMassShell();					// first, build the shell.
	produceOREsForShellPolys();					// build the OREs for each ECBPoly of the shell
	runMassDriversForIndependentMass();			// generate the mass.
}

void BPMassManagerV2::buildContourMassShell()
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

	for (auto& currentRegistry : contourPlanRef->allPolysRegistry.polySetRegistry)
	{
		EnclaveKeyDef::EnclaveKey currentFirstPassBlueprintKey = currentRegistry.first;
		OperableIntSet firstMassPassContourAddedTriangles = currentRegistry.second.polySet;
		copyMassShellPolysFromServerToMass(currentFirstPassBlueprintKey, firstMassPassContourAddedTriangles);
	}
}

void BPMassManagerV2::copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet)
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

void BPMassManagerV2::produceOREsForShellPolys()
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

void BPMassManagerV2::runMassDriversForIndependentMass()
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

OrganicTriangleTracker* BPMassManagerV2::getReformerTrackerRef(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	return &reformerTracker[in_blueprintKey].triangleTracker;
}

void BPMassManagerV2::buildPersistentMasses()
{
	auto contouredBlueprintsBegin = contouredPlanMass.getBlueprintBeginIter();
	auto contouredBlueprintsEnd = contouredPlanMass.getBlueprintEndIter();

	for (; contouredBlueprintsBegin != contouredBlueprintsEnd; contouredBlueprintsBegin++)
	{
		OperableIntSet contouredPlanAddedTrianglesToCurrentBlueprint = contourPlanRef->allPolysRegistry.polySetRegistry[contouredBlueprintsBegin->first].polySet;
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

void BPMassManagerV2::scanForDissolvableTriangles()
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

void BPMassManagerV2::updatePersistentBlueprintPolys()
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

			auto currentPolyShatteredCheck = ecbPolyReformerTrackerBegin->second.firstPassResultingShatteredECBPolys.find(*currentReformerContouredSetBegin);
			if (currentPolyShatteredCheck != ecbPolyReformerTrackerBegin->second.firstPassResultingShatteredECBPolys.end())
			{
				// now, take each produced contoured (shattered) ECBPoly from the corresponding ShatteredECBPolys instance, and add it to the persistent blueprint's ECBPolys.
				auto currentShatteredPolysBegin = currentPolyShatteredCheck->second.shatteredEcbPolyMap.begin();
				auto currentShatteredPolysEnd = currentPolyShatteredCheck->second.shatteredEcbPolyMap.end();
				//std::cout << "Origin ECBPoly ID is: " << currentPolyShatteredCheck->first << std::endl;
				for (; currentShatteredPolysBegin != currentShatteredPolysEnd; currentShatteredPolysBegin++)
				{
					ecbPolyReformerTrackerBegin->second.serverBlueprintRef->insertPolyWithKeyValue(currentShatteredPolysBegin->first, currentShatteredPolysBegin->second);
				}

				// remove the ECBPoly that these contoured (shattered) ECBPoly(s) were spawned from, in the persistent map.
				ecbPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(currentPolyShatteredCheck->first);

				// lastly, for each entry in the current reformer's firstPassShatteredOreSet, update that corresponding ORE having the same key in the server (persistent) blueprint,
				// as being INDEPENDENT.
				auto firstPassShatteredOreSetBegin = ecbPolyReformerTrackerBegin->second.firstPassShatteredORESet.begin();
				auto firstPassShatteredOreSetEnd = ecbPolyReformerTrackerBegin->second.firstPassShatteredORESet.end();
				for (; firstPassShatteredOreSetBegin != firstPassShatteredOreSetEnd; firstPassShatteredOreSetBegin++)
				{
					ecbPolyReformerTrackerBegin->second.serverBlueprintRef->fractureResults.fractureResultsContainerMap[*firstPassShatteredOreSetBegin].setOREasIndependent();
				}
			}

			auto currentPolyDeleteCheck = ecbPolyReformerTrackerBegin->second.firstPassUnshatteredECBPolysToErase.find(*currentReformerContouredSetBegin);
			if (currentPolyDeleteCheck != ecbPolyReformerTrackerBegin->second.firstPassUnshatteredECBPolysToErase.end())
			{
				ecbPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(*currentReformerContouredSetBegin);
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

			auto currentPersistentPolyShatteredCheck = secondPassECBPolyReformerTrackerBegin->second.secondPassResultingShatteredECBPolys.find(*currentReformerPersistentSetBegin);
			if (currentPersistentPolyShatteredCheck != secondPassECBPolyReformerTrackerBegin->second.secondPassResultingShatteredECBPolys.end())
			{
				// now,  take each produced persistent (shatered) ECBPoly from the corressponding ShatteredECBPolys instance, and add it to the persistent blueprint's ECBPolys.

				auto currentPersistentShatteredPolysBegin = currentPersistentPolyShatteredCheck->second.shatteredEcbPolyMap.begin();
				auto currentPersistentShatteredPolysEnd = currentPersistentPolyShatteredCheck->second.shatteredEcbPolyMap.end();
				for (; currentPersistentShatteredPolysBegin != currentPersistentShatteredPolysEnd; currentPersistentShatteredPolysBegin++)
				{
					// need to figure out why the below line was commented out before 8/27/2021...(perhaps a mistake?)
					secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->insertPolyWithKeyValue(currentPersistentShatteredPolysBegin->first, currentPersistentShatteredPolysBegin->second);
				}

				// remove the ECBPoly that these persistent (shattered) ECBPoly(s) were spawned from, in the persistent map.
				secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->deletePoly(currentPersistentPolyShatteredCheck->first);

				// lastly, for each entry in the current reformer's secondPassShatteredOreSet, update that corresponding ORE having the same key in the server (persistent) blueprint,
				// as being INDEPENDENT.
				auto secondPassShatteredOreSetBegin = secondPassECBPolyReformerTrackerBegin->second.secondPassShatteredORESet.begin();
				auto secondPassShatteredOreSetEnd = secondPassECBPolyReformerTrackerBegin->second.secondPassShatteredORESet.end();
				for (; secondPassShatteredOreSetBegin != secondPassShatteredOreSetEnd; secondPassShatteredOreSetBegin++)
				{
					secondPassECBPolyReformerTrackerBegin->second.serverBlueprintRef->fractureResults.fractureResultsContainerMap[*secondPassShatteredOreSetBegin].setOREasIndependent();
				}

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