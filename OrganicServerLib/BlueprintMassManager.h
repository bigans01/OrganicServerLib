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
#include "ContourBase.h"
#include "ECBPolyReformer.h"

class BlueprintMassManager
{
	public:
		BlueprintMassManager() {};


		BlueprintMassManager(ECBMap* in_managerEcbMapRef,
			OrganicClient* in_organicClientRef,
			ContourBase* in_contourPlanRef) :
			managerEcbMapRef(in_managerEcbMapRef),
			organicClientRef(in_organicClientRef),
			contourPlanRef(in_contourPlanRef)
		{};

		void buildContouredMass();

		void buildPersistentMasses();		// build persistent masses for any blueprints found to exist on the server, that also exist in the ContourPlan.
		OrganicTriangleTracker* getReformerTrackerRef(EnclaveKeyDef::EnclaveKey in_blueprintKey);
		void scanForDissolvableTriangles();
		void updatePersistentBlueprintPolys();

		void checkStateOfSpecificOre(EnclaveKeyDef::EnclaveKey in_blueprintKeyToCheck,
			EnclaveKeyDef::EnclaveKey in_oreKeyToCheck);

		ECBMap contouredPlanMass;
	private:
		ECBMap* managerEcbMapRef = nullptr;

		ContourBase* contourPlanRef = nullptr;
		OrganicClient* organicClientRef = nullptr;

		ForgedPolyRegistry contouredPlanEntireShellRegistry;
		ForgedPolyRegistry contouredPlanMassDriverRegistry;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OperableIntSet, EnclaveKeyDef::KeyHasher> contouredPlanShellBlueprintKeyAndSetPairs;

		ECBMap persistentMass;	// the mass for what exists on the server, before the contour plan adds anything.

		std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBPolyReformer, EnclaveKeyDef::KeyHasher> reformerTracker;

		void buildContourMassShell();		// build a mass shell, based off the shell spolys that the referenced contour plan produced.
		void produceOREsForShellPolys();
		void runMassDriversForIndependentMass();

		// copy the ECBPolys that were added by the referenced ContourPlan to a specific existing blueprint, into this BlueprintMassManager instance.
		void copyMassShellPolysFromServerToMass(EnclaveKeyDef::EnclaveKey in_blueprintKey, OperableIntSet in_contourAddedOrganicTrianglesSet);	
};

#endif
