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
#include "OrganicClient.h"
#include "ContourBase.h"

class BlueprintMassManager
{
	public:
		BlueprintMassManager() {};
		BlueprintMassManager(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_serverBlueprintsRef,
			                 OrganicClient* in_organicClientRef,
			                 ContourBase* in_contourPlanRef) :
			serverBlueprintsRef(in_serverBlueprintsRef),
			organicClientRef(in_organicClientRef),
			contourPlanRef(in_contourPlanRef)
		{};

		void buildContouredMass();

		void buildPersistentMasses();		// build persistent masses for any blueprints found to exist on the server, that also exist in the ContourPlan.
	private:
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* serverBlueprintsRef = nullptr;

		ContourBase* contourPlanRef = nullptr;
		OrganicClient* organicClientRef = nullptr;

		ECBMap contouredPlanMass;
		ForgedPolyRegistry contouredPlanEntireShellRegistry;
		ForgedPolyRegistry contouredPlanMassDriverRegistry;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OperableIntSet, EnclaveKeyDef::KeyHasher> contouredPlanShellBlueprintKeyAndSetPairs;

		ECBMap persistentMass;	// the mass for what exists on the server, before the contour plan adds anything.


		void buildContourMassShell();		// build a mass shell, based off the shell spolys that the referenced contour plan produced.
		void produceOREsForShellPolys();
		void runMassDriversForIndependentMass();

		// copy the ECBPolys that were added by the referenced ContourPlan to a specific existing blueprint, into this BlueprintMassManager instance.
		void copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet);	
};

#endif
