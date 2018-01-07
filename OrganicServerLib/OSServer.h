#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSContourPlan.h"
#include "EnclaveCollectionBlueprint.h"
#include "EnclaveKeyDef.h"
#include "ECBCarvePointArray.h"
#include <unordered_map>
#include <string>



class OSServer
{
public:
	
	OSServer();
	OSServer(int x);

	void addContourPlan(string in_string, float in_x, float in_y, float in_z);		// adds a plan to contourPlanMap
	int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);					// returns 1 if blueprint exists
	void executeContourPlan(string in_string);										// executes operations for all triangle strips in a triangle plan
	OSContourPlan* getContourPlan(string in_string);								// return a pointer to a valid contourPlan

private:
	std::unordered_map<string, OSContourPlan> contourPlanMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher> blueprintMap;	// stores all server blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointArray, EnclaveKeyDef::KeyHasher> carvePointArrayMap;		// stores all corresponding ECBCarvePointArrays for blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointList, EnclaveKeyDef::KeyHasher> carvePointListMap;		// stores all corresponding carvePointLists for blueprints
	void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle);
	OrganicSystem* organicSystemPtr;
};

#endif