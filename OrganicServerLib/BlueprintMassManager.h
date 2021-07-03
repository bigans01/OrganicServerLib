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
		void buildContourMassShell();		// build a mass shell, based off the shell spolys that the referenced contour plan produced.
		void produceOREsForShellPolys();
		void runMassDriversForIndependentMass();
	private:
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* serverBlueprintsRef = nullptr;

		ContourBase* contourPlanRef = nullptr;
		OrganicClient* organicClientRef = nullptr;
		ECBMap massA;
		ForgedPolyRegistry massAEntireShellRegistry;
		ForgedPolyRegistry massAMassDriverRegistry;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OperableIntSet, EnclaveKeyDef::KeyHasher> massAShellBlueprintKeyAndSetPairs;

		// copy the ECBPolys that were added by the referenced ContourPlan to a specific existing blueprint, into this BlueprintMassManager instance.
		void copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet);	
};

#endif
