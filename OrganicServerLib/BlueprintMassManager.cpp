#include "stdafx.h"
#include "BlueprintMassManager.h"

void BlueprintMassManager::transferMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet)
{
	EnclaveCollectionBlueprint* currentBlueprintRef = &(*serverBlueprintsRef)[in_blueprintKey];
	massAShellBlueprintKeyAndSetPairs[in_blueprintKey] = in_contourAddedOrganicTrianglesSet;
	std::map<int, ECBPoly>* currentPolyMapRef = &currentBlueprintRef->primaryPolygonMap;
	auto setBegin = in_contourAddedOrganicTrianglesSet.intSet.begin();
	auto setEnd = in_contourAddedOrganicTrianglesSet.intSet.end();
	for (; setBegin != setEnd; setBegin++)
	{
		ECBPoly* polyRef = &(*currentPolyMapRef).find(*setBegin)->second;	// grab a ref from the persistent state of the blueprint on the server
		massAEntireShellRegistry.addToPolyset(in_blueprintKey, *setBegin);// add to the entire shell
		if (polyRef->polyType == ECBPolyType::SHELL_MASSDRIVER)					// if it's a shell mass driver, insert it into the massAMassDriverRegistry
		{
			massAMassDriverRegistry.addToPolyset(in_blueprintKey, *setBegin);
		}

		// create the blueprint in massA, if it doesn't exist (it shouldn't, but the check is for safety)
		massA.insertBlueprintIfNonExistent(in_blueprintKey);					
		EnclaveCollectionBlueprint* massABlueprintRef = massA.getBlueprintRef(in_blueprintKey);	
		currentBlueprintRef->transferExistingECBPolysToOtherBlueprint(in_contourAddedOrganicTrianglesSet, massABlueprintRef);

	}
}

void BlueprintMassManager::produceOREsForShellPolys(OrganicClient* in_clientRef)
{
	auto massShellBegin = massAShellBlueprintKeyAndSetPairs.begin();
	auto massShellEnd = massAShellBlueprintKeyAndSetPairs.end();
	for (; massShellBegin != massShellEnd; massShellBegin++)
	{
		in_clientRef->OS->produceRawEnclavesForPolySet(massA.getFractureResultsMapRef(massShellBegin->first),
														massShellBegin->first,
														massA.getBlueprintRef(massShellBegin->first),
														massShellBegin->second.intSet);
	}
}

void BlueprintMassManager::runMassDriversForIndependentMass(OrganicClient* in_clientRef)
{
	auto massDriverRegistryBegin = massAMassDriverRegistry.polySetRegistry.begin();
	auto massDriverRegistryEnd = massAMassDriverRegistry.polySetRegistry.end();
	for (; massDriverRegistryBegin != massDriverRegistryEnd; massDriverRegistryBegin++)
	{
		// set up the set to use
		EnclaveKeyDef::EnclaveKey blueprintKey = massDriverRegistryBegin->first;
		ForgedPolySet originalSet = massAEntireShellRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set; for the entire outer shell
		//ForgedPolySet subtractingSet = massDriverRegistryBegin->second;
		ForgedPolySet massDriverSet = massDriverRegistryBegin->second;				// the set that represents the polys identified as SHELL_MASSDRIVER type.
		ForgedPolySet startingFloorTerminatingSet = originalSet;

		auto subtractionBegin = massDriverSet.polySet.begin();
		auto subtractionEnd = massDriverSet.polySet.end();
		for (subtractionBegin; subtractionBegin != subtractionEnd; subtractionBegin++)
		{
			startingFloorTerminatingSet.polySet.erase(*subtractionBegin);
		}

		in_clientRef->OS->generateAndRunMassDriversForBlueprint(massA.getBlueprintMapRef(),
																&massAEntireShellRegistry.polySetRegistry,
																blueprintKey,
																startingFloorTerminatingSet,
																massDriverSet);
	}
}