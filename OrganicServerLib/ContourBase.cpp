#include "stdafx.h"
#include "ContourBase.h"

void ContourBase::runPolyFracturerForAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{
	std::cout << "####### Running poly fracturer for affected blueprints!!! " << std::endl;
	auto affectedBegin = planPolyRegistry.polySetRegistry.begin();
	auto affectedEnd = planPolyRegistry.polySetRegistry.end();
	for (affectedBegin; affectedBegin != affectedEnd; affectedBegin++)
	{
		EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[affectedBegin->first];
		//client.fracturePolysInBlueprint(bpBegin->first, &bpBegin->second, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
		//in_clientRef->fracturePolysInBlueprint(affectedBegin->first, blueprintRef, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
		in_clientRef->fractureSelectivePolysInBlueprint(affectedBegin->first, blueprintRef, &planPolyRegistry, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
	}
}

void ContourBase::writeAffectedBlueprintsToDisk(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, std::string in_worldName)
{
	auto affectedBegin = planPolyRegistry.polySetRegistry.begin();
	auto affectedEnd = planPolyRegistry.polySetRegistry.end();
	for (affectedBegin; affectedBegin != affectedEnd; affectedBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentFileName = affectedBegin->first;	// get the blueprint traversed
		//std::cout << ">> Blueprint file to write is: " << currentFileName.x << ", " << currentFileName.y << ", " << currentFileName.z << ", " << std::endl;
		EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[currentFileName];

		// values to load to the strucutre that gets passed to the adapter 
		std::map<int, ECBPoly>* polyMapRef = &blueprintRef->primaryPolygonMap;


		OSWinAdapter::writeBlueprintPolysToFile(in_worldName, currentFileName, polyMapRef);

		//EnclaveCollectionBlueprint readBackBP;
		//OSWinAdapter::readBlueprintPolysFromFile(in_worldName, currentFileName, polyMapRef);
		OSWinAdapter::outputBlueprintStats(polyMapRef);
	}
}