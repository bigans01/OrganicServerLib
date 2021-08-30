#include "stdafx.h"
#include "ContourBase.h"

void ContourBase::runPolyFracturerForAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{
	//std::cout << "####### Running poly fracturer for affected blueprints!!! " << std::endl;
	auto affectedBegin = planPolyRegistry.polySetRegistry.begin();
	auto affectedEnd = planPolyRegistry.polySetRegistry.end();
	for (affectedBegin; affectedBegin != affectedEnd; affectedBegin++)
	{
		EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[affectedBegin->first];
		//client.fracturePolysInBlueprint(bpBegin->first, &bpBegin->second, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
		//in_clientRef->fracturePolysInBlueprint(affectedBegin->first, blueprintRef, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
		in_clientRef->fractureSelectivePolysInBlueprint(affectedBegin->first, blueprintRef, &planPolyRegistry, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
	}
	//std::cout << "####### Poly fracturing complete. " << std::endl;
}

void ContourBase::writeAffectedBlueprintsToDisk(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, std::string in_worldName)
{
	auto organicstart = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	auto affectedBegin = planPolyRegistry.polySetRegistry.begin();
	auto affectedEnd = planPolyRegistry.polySetRegistry.end();
	int writeCount = 0;
	for (affectedBegin; affectedBegin != affectedEnd; affectedBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentFileName = affectedBegin->first;	// get the blueprint traversed
		//std::cout << ">> Blueprint file to write is: " << currentFileName.x << ", " << currentFileName.y << ", " << currentFileName.z << ", " << std::endl;
		EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[currentFileName];
		//std::map<int, ECBPoly>* polyMapRef = &blueprintRef->primaryPolygonMap;	// values to load to the strucutre that gets passed to the adapter 

		//OSWinAdapter::writeBlueprintPolysToFile(in_worldName, currentFileName, polyMapRef);

		BlueprintTransformRefs transformRefs(blueprintRef->getPolygonMapBeginIter(),
											blueprintRef->getPolygonMapEndIter(),
											blueprintRef->getPolygonMapSize(),
											&blueprintRef->fractureResults.fractureResultsContainerMap,
											&blueprintRef->polyGroupRangeMap);
		OSWinAdapter::writeBlueprintsToFile(in_worldName, currentFileName, transformRefs);


		//EnclaveCollectionBlueprint readBackBP;
		//OSWinAdapter::readBlueprintPolysFromFile(in_worldName, currentFileName, polyMapRef);
		//OSWinAdapter::outputBlueprintStats(polyMapRef);
		writeCount++;
	}
	auto organicend = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	std::chrono::duration<double> organicelapsed = organicend - organicstart;

	//std::cout << "Frame time: " << organicelapsed.count() << std::endl;

	std::cout << "Wrote " << writeCount << " blueprints to disk, in " << organicelapsed.count() << " seconds. " << std::endl;
	//int continueVal = 3;
	//std::cin >> continueVal;
}

void ContourBase::updateAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef)
{
	auto planPolyRegistryBegin = planPolyRegistry.polySetRegistry.begin();
	auto planPolyRegistryEnd = planPolyRegistry.polySetRegistry.end();
	for (planPolyRegistryBegin; planPolyRegistryBegin != planPolyRegistryEnd; planPolyRegistryBegin++)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin->first;
		EnclaveCollectionBlueprint* blueprintToCheck = &(*in_blueprintMapRef)[blueprintKey];
		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		//EnclaveFractureResultsMap tempMap;
		//in_clientRef->OS->produceRawEnclavesForPolySet(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet);		// first, generate the OrganicRawEnclaves that would be produced by this set
		//in_clientRef->OS->updateRawEnclaveData(in_fractureResultsMapRef, blueprintToCheck);
	}
}

ECBPolyPoint ContourBase::roundContourPointToHundredths(ECBPolyPoint in_contourPoint)
{
	ECBPolyPoint returnPoint;
	returnPoint.x = (floor(in_contourPoint.x * 100)) / 100;
	returnPoint.y = (floor(in_contourPoint.y * 100)) / 100;
	returnPoint.z = (floor(in_contourPoint.z * 100)) / 100;
	return returnPoint;
}

void ContourBase::insertPreferredMaterial(int in_materialID)
{
	int currentLookupSize = preferredMaterialLookup.size();
	preferredMaterialLookup[currentLookupSize] = in_materialID;
}

int ContourBase::getPreferredMaterialAtIndex(int in_indexToLookup)
{
	int returnMaterialID = 1;	// 1, which is DIRT -- should be returned if nothing is found.
	if (auto materialFinder = preferredMaterialLookup.find(in_indexToLookup); materialFinder != preferredMaterialLookup.end())
	{
		returnMaterialID = materialFinder->second;
	}
	return returnMaterialID;
}