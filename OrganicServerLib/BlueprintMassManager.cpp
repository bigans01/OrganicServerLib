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
		persistentBlueprintAllTriangles -= contouredPlanAddedTrianglesToCurrentBlueprint;
		if 
		(
			(!contouredPlanAddedTrianglesToCurrentBlueprint.intSet.empty())
			&&
			(!persistentBlueprintAllTriangles.intSet.empty())
		)
		{
			std::cout << "Notice! persistent addition will be required into the persistentMass. Key:(" << contouredBlueprintsBegin->first.x << ", "
				<< contouredBlueprintsBegin->first.y << ", "
				<< contouredBlueprintsBegin->first.z << ") " << std::endl;
			EnclaveCollectionBlueprint* blueprintFractureCopyDestinationRef = persistentMass.getBlueprintRef(contouredBlueprintsBegin->first);
			EnclaveCollectionBlueprint* blueprintFractureCopySourceRef = &(*serverBlueprintsRef)[contouredBlueprintsBegin->first];
			blueprintFractureCopySourceRef->copyFractureResultsMapToOtherBlueprint(blueprintFractureCopyDestinationRef);

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