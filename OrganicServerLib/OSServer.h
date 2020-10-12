#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSContourPlan.h"
#include "OSTrianglePoint.h"
#include "OSTriangleLineTraverser.h"
#include "OSTerrainFormation.h"
#include "OSCommandDirector.h"
#include "OrganicStemcellManager.h"
#include "OSContouredTriangleRunner.h"
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
#include "OSWinAdapter.h"
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include "OrganicClient.h"
#include "ServerProperties.h"
#include "TriangleLine.h"
#include "PolyDebugLevel.h"
#include "ContourBase.h"
#include "ContouredMountain.h"
#include "BlankContour.h"
#include "PointAdherenceOrder.h"
#include "MessageCable.h"
#include "ServerMessageInterpreter.h"
#include "ServerJobManager.h"
#include "ContourPlanStateContainer.h"


class OSServer
{
public:
	ServerMessageInterpreter messageInterpreter;
	MessageCable serverMessages;
	OrganicClient client;
	std::shared_ptr<OrganicSystem> organicSystemPtr;
	ServerJobManager serverJobManager;
	OrganicStemcellManager OSCManager;
	OSCommandDirector OSdirector;
	short isServerActive = 1;			// flag for determining server
	short numberOfSlaves = 0;			// number of slave threads
	int serverRunMode = 0;				// will be set in constructor
	int isCommandLineRunning = 1;
	int isCommandLineShutDown = 0;		// is the commandLine shutdown?
	std::string currentWorld;
	std::mutex serverReadWrite;			// the server's mutex for reading/writing into it's variables
	std::mutex commandLineRunningMutex;	// mutex for when the command line runs
	std::condition_variable commandLineCV;

	OSServer(int numberOfFactories, int T1_bufferCubeSize, int T2_bufferCubeSize, int windowWidth, int windowHeight, int serverMode, int serverSlaves);		// manual startup specification
	OSServer();							// will read from server properties file to start
	~OSServer();						// destructor; required for deletion of threads

	void runServer();					// runs the server, after the command line has been set up.
	void executeCommandLine();			// runs the command line
	void addDerivedContourPlan(string in_planName, OSTerrainFormation in_Formation, ECBPolyPoint in_polyPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
	int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);									// returns 1 if blueprint exists (requires heap mutex)

	// blueprint testing functions
	void constructTestBlueprints();		// CONVERTED 
	void constructTestBlueprints2();	// CONVERTED
	void constructTestBlueprints3();	// CONVERTED		// for mountain testing; 
	void constructMountainAtPoint(float in_summitX, float in_summitY, float in_summitZ, int numberOfLayers);
	void constructTestBlueprints3Special();	// CONVERTED		// for mountain testing; 
	void constructTestBlueprints3Micro();

	void constructTestDisk();
	void constructMiniTestDisk();
	void constructSingleDebug();		// special case. one triangle.

	void constructTestBlueprints5();	
	void constructTestBlueprintsForFracturing();	// CONVERTED	// for fracturing mountain testing (for now)
	void constructTestBlueprintsForFracturing2();	// CONVERTED	// for fracturing mountain testing (for now)
	void constructTestBlueprintsForContourBaseTesting();	// CONVERTED, for fracturing mountain testing (for now)
	void constructTestBlueprints4();	// CONVERTED // side testing
	void constructDebugBlueprint1();	// CONVERTED
	void constructBlueprintFillTest();
	void constructSingleMountTest();
	void constructMultiMountTest();
	void constructMissingFillBlueprint3();
	void constructSingleOrganicTest();
	void constructOrganicRawTest();

	//void executeContourPlan(string in_string);	// executes operations for all triangle strips in a triangle plan
	void executeDerivedContourPlan(string in_string);
	void executeDerivedContourPlanNoInput(string in_string);	// run the plan without waiting for input afterwards.

	void sendAndRenderBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
	void sendAndRenderAllBlueprintsToLocalOS();											// transfers all processed blueprints to the local OS.
	void setWorldDirectionInClient(float in_directionX, float in_directionY, float in_directionZ);

	ContourBase* getDerivedContourPlan(string in_string);
	void transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
	void runPolyFracturer(EnclaveKeyDef::EnclaveKey in_key, PolyDebugLevel in_debugLevel);							// testing only (for now)
	void runPolyFracturerForAllBlueprints();

	void constructBlueprintFromFile(std::string in_worldName, EnclaveKeyDef::EnclaveKey in_blueprintKey);
	void checkClientMessages();
private:
	friend class OSTriangleLineTraverser;
	friend class ServerMessageInterpreter;
	friend class ServerJobManager;
	friend class ServerJobProxy;

	std::unordered_map<string, std::unique_ptr<ContourBase>> newContourMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher> blueprintMap;	// stores all server blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointArray, EnclaveKeyDef::KeyHasher> carvePointArrayMap;		// stores all corresponding ECBCarvePointArrays for blueprints
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBCarvePointList, EnclaveKeyDef::KeyHasher> carvePointListMap;		// stores all corresponding carvePointLists for blueprints
	ContourPlanStateContainer planStateContainer;

	void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions, PointAdherenceOrder* in_orderRef);		// constructs primary polygon lines for each line of the contoured triangle that the 
	void writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys);
	void analyzeECBPoly(ECBPoly* in_polyRef);
	void setCurrentWorld(std::string in_worldName);
	int runCommandLine(mutex& in_serverReadWrite, std::condition_variable& in_conditionVariable, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus);		// may be deprecated eventually, replaced by runCommandLineV3. Deprecation validity tests began 9/29/2020.
	int runCommandLineV2(mutex& in_serverReadWrite, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus);													// may be deprecated eventually, replaced by runCommandLineV3. Deprecation validity tests began 9/29/2020.
	void runCommandLineV3();
	int checkServerStatus(mutex& in_serverReadWrite);
	void setServerStatus(mutex& in_serverReadWrite, int in_valueToSet, int* in_commandLineStatus);
	void signalCommandLineShutdown(mutex& in_serverReadWrite, int in_valueToSet, int* in_clShutdownFlag);
	int getCommandLineShutdownValue(mutex& in_serverReadWrite);
	void signalServerShutdown(mutex& in_serverMutex);
	OSPDir getFormationDirections(OSTerrainFormation in_terrainFormation);

	// test run jobs for ServerJobManager
	void constructSingleMountTestNoInput();
	void constructBigMountTestNoInput();
	void jobSendSetWorldDirectionToClient(Message in_message);
	void jobSendUpdateMessageToJobManager(Message in_message);
	void jobSendRequestToSendOGLMCubeFromClient();

};

#endif