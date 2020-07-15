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