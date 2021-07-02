#pragma once

#ifndef BLUEPRINTMASSMANAGER_H
#define BLUEPRINTMASSMANAGER_H

#include "EnclaveKeyDef.h"
#include "ECBMap.h"
#include "ForgedPolySet.h"
#include "OperableIntSet.h"
#include "ForgedPolyRegistry.h"
#include "OrganicClient.h"
#include <unordered_map>

class BlueprintMassManager
{
	public:
		BlueprintMassManager() {};
		BlueprintMassManager(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_serverBlueprintsRef) :
			serverBlueprintsRef(in_serverBlueprintsRef)
		{};
		void transferMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet);
		void produceOREsForShellPolys(OrganicClient* in_clientRef);
		void runMassDriversForIndependentMass(OrganicClient* in_clientRef);
	private:
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* serverBlueprintsRef = nullptr;

		ECBMap massA;
		ForgedPolyRegistry massAEntireShellRegistry;
		ForgedPolyRegistry massAMassDriverRegistry;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OperableIntSet, EnclaveKeyDef::KeyHasher> massAShellBlueprintKeyAndSetPairs;

};

#endif
