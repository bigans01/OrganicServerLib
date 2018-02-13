#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSContourPlan.h"
#include "OSTrianglePoint.h"
#include "OSTriangleLineTraverser.h"
#include "OSPDir.h"
#include "EnclaveCollectionBlueprint.h"
#include "OSContourPlanDirections.h"
#include "EnclaveKeyDef.h"
#include "ECBBorderLineList.h"
#include "ECBCarvePointArray.h"
#include "ECBPoly.h"
#include "ECBPolyLine.h"
#include "ECBPolyPointTri.h"
#include "EnclaveKeyTri.h"
#include <unordered_map>
#include <string>
#include <chrono>



class OSServer
{
public:
	
	OSServer();
	OSServer(int x);

	void addContourPlan(string in_planName, OSPDir in_Dir, float in_x, float in_y, float in_z);		// adds a plan to contourPlanMap
	int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);					// returns 1 if blueprint exists
	void executeContourPlan(string in_string);										// executes operations for all triangle strips in a triangle plan
	OSContourPlan* getContourPlan(string in_string);								// return a pointer to a valid contourPlan
	friend class OSTriangleLineTraverser;

private:
	std::unordered_map<string, OSContourPlan> contourPlanMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher> blueprintMap;	// stores all server blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointArray, EnclaveKeyDef::KeyHasher> carvePointArrayMap;		// stores all corresponding ECBCarvePointArrays for blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointList, EnclaveKeyDef::KeyHasher> carvePointListMap;		// stores all corresponding carvePointLists for blueprints
	void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void determineTriangleRelativityToECB(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void findTrueKey(OSContouredTriangle* in_Triangle, OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList);
	void determineTriangleCentroid(OSContouredTriangle* in_Triangle);
	void determineTriangleType2and3Lines(OSContouredTriangle* in_Triangle);
	void rayCastTrianglePoints(OSContouredTriangle* in_Triangle);
	void tracePointThroughBlueprints(OSContouredTriangle* in_Triangle, int in_pointID);
	void constructTestBlueprints();
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID);
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint);
	OrganicSystem* organicSystemPtr;
};

#endif