#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSContourPlan.h"
#include "OSTrianglePoint.h"
#include "OSTriangleLineTraverser.h"
#include "OSCommandDirector.h"
#include "OrganicStemcellManager.h"
#include "OSPDir.h"
#include "OrganicThreadController.h"
#include "EnclaveCollectionBlueprint.h"
#include "OSContourPlanDirections.h"
#include "EnclaveKeyDef.h"
#include "ECBBorderLineList.h"
#include "ECBCarvePointArray.h"
#include "ECBPoly.h"
#include "ECBPolyLine.h"
#include "ECBPolyPointTri.h"
#include "EnclaveKeyTri.h"
#include "OSWinAdapter.h"
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>



class OSServer
{
public:
	
	OSServer();
	OSServer(int x);
	OSServer(OrganicSystem* in_organicSystemPtr, int in_numberOfSlaves, int in_serverRunMode);
	~OSServer();						// destructor; required for deletion of threads
	void addContourPlan(string in_planName, OSPDir in_Dir, float in_x, float in_y, float in_z);		// adds a plan to contourPlanMap	(requires heap mutex)
	int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);									// returns 1 if blueprint exists (requires heap mutex)
	void constructTestBlueprints();
	void executeContourPlan(string in_string);	// executes operations for all triangle strips in a triangle plan
	void transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
	OSContourPlan* getContourPlan(string in_string);								// return a pointer to a valid contourPlan
	OrganicSystem* organicSystemPtr;
	OrganicStemcellManager OSCManager;
	OSCommandDirector OSdirector;
	friend class OSTriangleLineTraverser;
	short isServerActive = 1;			// flag for determining server
	short numberOfSlaves = 0;			// number of slave threads
	int serverRunMode = 0;				// will be set in constructor
	int isCommandLineRunning = 1;
	int isCommandLineShutDown = 0;		// is the commandLine shutdown?
	std::string currentWorld;
	void runServer();					// runs the server, after the command line has been set up.
	void executeCommandLine();			// runs the command line
	std::mutex serverReadWrite;			// the server's mutex for reading/writing into it's variables
	std::mutex commandLineRunningMutex;	// mutex for when the command line runs
	std::mutex* heapMutexRef;			// a reference to a stored heap mutex ref
	std::condition_variable commandLineCV;
private:
	std::unordered_map<string, OSContourPlan> contourPlanMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher> blueprintMap;	// stores all server blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointArray, EnclaveKeyDef::KeyHasher> carvePointArrayMap;		// stores all corresponding ECBCarvePointArrays for blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointList, EnclaveKeyDef::KeyHasher> carvePointListMap;		// stores all corresponding carvePointLists for blueprints
	void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);		// constructs primary polygon lines for each line of the contoured triangle that the 
	void calibrateAndRunContouredTriangle(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys);
	void analyzeECBPoly(ECBPoly* in_polyRef);
	void setCurrentWorld(std::string in_worldName);
	void calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions);
	void findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList);
	void determineTriangleCentroid(OSContouredTriangle* in_Triangle);
	void determineTriangleType2and3Lines(OSContouredTriangle* in_Triangle);		// currently unused, should be axed at a later time
	void rayCastTrianglePoints(OSContouredTriangle* in_Triangle);				// after the lines of a contoured triangle have been calibrated, this function traces the lines through the "world" and adds 1 new polygon (along with 1 primary line in that polygon) for each blueprint the line passes through.
	void tracePointThroughBlueprints(OSContouredTriangle* in_Triangle, int in_pointID);		// traces a point (line) through the blueprints it will traverse (requires heap mutex)
	int runCommandLine(mutex& in_serverReadWrite, std::condition_variable& in_conditionVariable, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus);
	int checkServerStatus(mutex& in_serverReadWrite);
	void setServerStatus(mutex& in_serverReadWrite, int in_valueToSet, int* in_commandLineStatus);
	void signalCommandLineShutdown(mutex& in_serverReadWrite, int in_valueToSet, int* in_clShutdownFlag);
	int getCommandLineShutdownValue(mutex& in_serverReadWrite);
	void signalServerShutdown(mutex& in_serverMutex);
	static void fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle);
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID);
	static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint);
};

#endif