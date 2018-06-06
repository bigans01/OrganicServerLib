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
#include <mutex>
#include "thread_pool.h"



class OSServer
{
public:
	
	OSServer();
	OSServer(int x);
	OSServer(OrganicSystem* in_organicSystemPtr, int in_numberOfSlaves, int in_serverRunMode);
	~OSServer();						// destructor; required for deletion of threads
	void addContourPlan(string in_planName, OSPDir in_Dir, float in_x, float in_y, float in_z);		// adds a plan to contourPlanMap
	int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);					// returns 1 if blueprint exists
	void constructTestBlueprints();
	void executeContourPlan(string in_string);										// executes operations for all triangle strips in a triangle plan]
	void transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
	OSContourPlan* getContourPlan(string in_string);								// return a pointer to a valid contourPlan
	OrganicSystem* organicSystemPtr;
	friend class OSTriangleLineTraverser;
	short isServerActive = 1;			// flag for determining server
	short numberOfSlaves = 0;			// number of slave threads
	int serverRunMode = 0;				// will be set in constructor
	void runServer();					// runs the server, after the command line has been set up.
	void executeCommandLine();			// runs the command line
	thread_pool* organicServerSlaves[16];	// up to 16 "slave" threads
	std::mutex serverReadWrite;			// the server's mutex for reading/writing into it's variables

private:
	std::unordered_map<string, OSContourPlan> contourPlanMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher> blueprintMap;	// stores all server blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointArray, EnclaveKeyDef::KeyHasher> carvePointArrayMap;		// stores all corresponding ECBCarvePointArrays for blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointList, EnclaveKeyDef::KeyHasher> carvePointListMap;		// stores all corresponding carvePointLists for blueprints
	void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void determineTriangleRelativityToECB(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList);
	void determineTriangleCentroid(OSContouredTriangle* in_Triangle);
	void determineTriangleType2and3Lines(OSContouredTriangle* in_Triangle);
	void rayCastTrianglePoints(OSContouredTriangle* in_Triangle);
	void tracePointThroughBlueprints(OSContouredTriangle* in_Triangle, int in_pointID);
	void createSlaves();
	void deleteSlaves();
	int runCommandLine(mutex& in_serverReadWrite);
	//int getServerStatus();
	int checkServerStatus(mutex& in_serverReadWrite);
	void setServerStatus(mutex& in_serverReadWrite, int in_valueToSet);
	static void fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle);
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID);
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint);
};

#endif