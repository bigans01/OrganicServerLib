#include "stdafx.h"
#include "OSServer.h"
#include "OrganicUtils.h"
//#include "OSContourPlan.h"
//#include "shitTest.h"

OSServer::OSServer(int numberOfFactories, int T1_bufferCubeSize, int T2_bufferCubeSize, int windowWidth, int windowHeight, int serverMode, int serverSlaves)
{
	client.createOS(numberOfFactories, T1_bufferCubeSize, T2_bufferCubeSize, windowWidth, windowHeight, serverMode);		// create the client's OS
	organicSystemPtr = client.OS;																							// assign organicSystemPtr to the client's OS
	numberOfSlaves = serverSlaves;		// set number of server slaves
	serverRunMode = serverMode;		// set the run mode (0, 1, 2, 3) etc
	heapMutexRef = &organicSystemPtr->heapmutex;	// set the heap mutex; always use the OrganicSystem's heap mutex when running in this mode
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initialize(1, serverSlaves);			// signal for server mode, 2 threads
	OSdirector.initialize(this, std::ref(organicSystemPtr->heapmutex));
}

OSServer::OSServer()
{
	ServerProperties serverProperties = OSWinAdapter::readServerProperties();
	std::cout << "++++++Performing startup via pre-defined server properties++++++" << std::endl;
	std::cout << "Factories: " << serverProperties.factoryCount << std::endl;
	std::cout << "T1 size: " << serverProperties.T1_bufferCubeSize << std::endl;
	std::cout << "T2 size: " << serverProperties.T2_bufferCubeSize << std::endl;
	std::cout << "OpenGL width: " << serverProperties.windowWidth << std::endl;
	std::cout << "OpenGL height: " << serverProperties.windowHeight << std::endl;
	std::cout << "Run mode: " << serverProperties.runMode << std::endl;
	std::cout << "Server slaves: " << serverProperties.serverSlaves << std::endl;
	client.createOS(serverProperties.factoryCount, serverProperties.T1_bufferCubeSize, serverProperties.T2_bufferCubeSize, serverProperties.windowWidth, serverProperties.windowHeight, serverProperties.runMode);		// create the client's OS
	organicSystemPtr = client.OS;																							// assign organicSystemPtr to the client's OS
	numberOfSlaves = serverProperties.serverSlaves;		// set number of server slaves
	serverRunMode = serverProperties.runMode;		// set the run mode (0, 1, 2, 3) etc
	heapMutexRef = &organicSystemPtr->heapmutex;	// set the heap mutex; always use the OrganicSystem's heap mutex when running in this mode
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initialize(1, serverProperties.serverSlaves);			// signal for server mode, 2 threads
	OSdirector.initialize(this, std::ref(organicSystemPtr->heapmutex));
}

OSServer::~OSServer()
{
	//deleteSlaves();
}



void OSServer::addContourPlan(string in_planName, OSPDir in_Dir, float in_x, float in_y, float in_z)
{
	std::lock_guard<std::mutex> lock(*heapMutexRef);	// heap mutex lock
	OSContourPlan tempPlan(in_Dir, in_x, in_y, in_z);
	contourPlanMap[in_planName] = tempPlan;
}

void OSServer::addContourPlan(string in_planName, OSTerrainFormation in_Formation, ECBPolyPoint in_polyPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	std::cout << "Adding new contour plan... (style 2)" << std::endl;
	OSContourPlan tempPlan(in_polyPoint);
	contourPlanMap[in_planName] = tempPlan;
	OSContourPlan* tempPlanRef = &contourPlanMap[in_planName];
	tempPlanRef->setFormationBaseContourPoints(in_Formation, in_polyPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
}

void OSServer::constructTestBlueprints()
{
	//planRef->addContourLine(0, 15.0f, 70.0f, 8, passPoint);
	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	addContourPlan("plan", OSPDir::BELOW, -85.0f, 80.0f, 90.0f);
	OSContourPlan* planRef = getContourPlan("plan");		// get pointer to the plan

	// Triangle #1 point data
	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	testPoint_0.x = -42.0f;
	testPoint_0.y = 2.4f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 2.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 2.0f;
	testPoint_2.z = 2.0f;

	//testPoint_0.x = 64.0f;
	//testPoint_0.y = 0.0f;
	//testPoint_0.z = 0.0f;

	//testPoint_1.x = 28.0f;
	//testPoint_1.y = 0.0f;		// previously: -1.3f
	//testPoint_1.z = -3.0f;

	//testPoint_2.x = 69.0f;
	//testPoint_2.y = 33.0f;		// previously: 1.0f
	//testPoint_2.z = -10.0f;



	// X == Y test
	testPoint_0.x = 48.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 16.0f;

	testPoint_1.x = 16.0f;
	testPoint_1.y = -16.0f;		// previously: -1.3f
	testPoint_1.z = 49.0f;

	testPoint_2.x = 69.0f;
	testPoint_2.y = 33.0f;		// previously: 1.0f
	testPoint_2.z = -10.0f;



	// X == Z test (NEW CODE 2) OK
	testPoint_0.x = 48.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 16.0f;

	testPoint_1.x = 16.0f;
	testPoint_1.y = -15.8f;		// previously: -1.3f
	testPoint_1.z = 48.0f;

	testPoint_2.x = 69.0f;
	testPoint_2.y = 33.0f;		// previously: 1.0f
	testPoint_2.z = -10.0f;


	// Rotation test - (NEW CODE 2) OK
	//testPoint_0.x = -14.21f;		// previously: 4.0f
	//testPoint_0.y = 28.00f;
	//testPoint_0.z = 10.23f;

	//testPoint_1.x = 35.0f;			// previous: 35f
	//testPoint_1.y = 18.0f;
	//testPoint_1.z = 12.0f;

	//testPoint_2.x = 10.0f;			// previous:: 10f
	//testPoint_2.y = 10.0f;
	//testPoint_2.z = 13.5f;

	// right angle test - (NEW CODE 2) OK
	//testPoint_0.x = -12.0f;
	//testPoint_0.y = 10.0f;
	//testPoint_0.z = 10.0f;

	//testPoint_1.x = -4.0f;
	//testPoint_1.y = 10.0f;
	//testPoint_1.z = 10.0f;

	//testPoint_2.x = -4.0f;
	//testPoint_2.y = 16.0f;
	//testPoint_2.z = 16.0f;

	// clamped to border of collection - (NEW CODE 2) OK
	/*
	testPoint_0.x = -12.0f;
	testPoint_0.y = 10.0f;
	testPoint_0.z = 10.0f;

	testPoint_1.x = 0.0f;	// previous was -4.0f
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = 0.0f;	// previous was -4.0f
	testPoint_2.y = 16.0f;
	testPoint_2.z = 16.0f;
	*/

	// Rotation test - (NEW CODE 2) OK
	/*
	testPoint_0.x = -14.21f;
	testPoint_0.y = 28.00f;
	testPoint_0.z = 10.23f;

	testPoint_1.x = 45.5f;
	testPoint_1.y = -12.0f;			// previous: 18.0f
	testPoint_1.z = 20.0f;

	testPoint_2.x = 10.0f;
	testPoint_2.y = -10.0f;			// previous: 10.0f
	testPoint_2.z = 10.0f;
	*/

	// Right angle, test for OSContouredTriangle::determineLineAxisIntercept
	/*
	testPoint_0.x = -8.00f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 0.0f;
	testPoint_1.y = 8.0f;			// previous: 18.0f
	testPoint_1.z = 8.0f;

	testPoint_2.x = -8.0f;
	testPoint_2.y = 0.0f;			// previous: 10.0f
	testPoint_2.z = 8.0f;
	*/


	// For line/corner intersect test across blueprints
	// For tracing test
	//testPoint_0.x = -16.0f;
	//testPoint_0.y = 16.00f;		// previous: 7.0f
	//testPoint_0.z = 16.00f;		// previous: 0.0f

	//testPoint_1.x = 16.00f;
	//testPoint_1.y = 16.00f;			// previous: 18.0f
	//testPoint_1.z = 16.00f;

	//testPoint_2.x = 16.0f;
	//testPoint_2.y = -16.0f;			// previous: 10.0f
	//testPoint_2.z = 48.0f;



	/*
	// For tracing test (same faces) (PERFECT CLAMP, NOT READY FOR TESTING YET)
	testPoint_0.x = -27.0f;
	testPoint_0.y = 7.00f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.0f;
	testPoint_1.y = 7.00f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.0f;
	testPoint_2.y = 7.0f;			// previous: 10.0f
	testPoint_2.z = 27.0f;
	*/

	/**/

	// TEST 00: for common T1 points?? test (FIXED? (maybe), 7/21/2018)
	/*
	testPoint_0.x = -26.8f;
	testPoint_0.y = 7.00f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.23f;
	testPoint_1.y = 7.00f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 15.00f;			// previous: 10.0f
	testPoint_2.z = 27.00f;
	*/

	// TEST 00-B:for common T1 points?? was error on 7/21/2018, but became new issue when filling triangle interior
	// new issue: infinite filling when using interior fill 

	/*
	testPoint_0.x = -26.8f;
	testPoint_0.y = 7.00f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.23f;
	testPoint_1.y = 7.23f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 15.00f;			// previous: 10.0f
	testPoint_2.z = 27.00f;
	*/

	/*
	// TEST 00-C: original points that caused error TEST 00-B
	testPoint_0.x = -26.8f;
	testPoint_0.y = 7.23f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.23f;															// <<------ crash is with this line (point 1 to point 2)
	testPoint_1.y = 7.23f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 15.23f;			// previous: 10.0f
	testPoint_2.z = 27.00f;
	*/

	// TEST 01: triangle clamped on one line



	// TEST 01: triangle clamped on one line; For tracing test (same faces)	 (Issue resolved on 7/22/2018)
	/*
	testPoint_0.x = -27.0f;
	testPoint_0.y = 7.00f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.0f;
	testPoint_1.y = 7.00f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.00f;
	testPoint_2.y = 15.00f;			// previous: 10.0f
	testPoint_2.z = 27.00f;
	*/


	//  TEST 02: non-clamped triangle with 3 points

	testPoint_0.x = -26.4f;
	testPoint_0.y = 6.78f;		// previous: 7.0f		// error with 6.78 here (7/5/2018) --> fixed, 7/9/2018
	testPoint_0.z = 1.00f;		// previous: 0.0f		

	testPoint_1.x = -10.2f;
	testPoint_1.y = 20.80f;			// previous: 18.0f			(7.80, 10.80, 20.80f); 20.80f leads to interesting resutls/angles
	testPoint_1.z = 4.70f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 7.45f;			// previous: 10.0f
	testPoint_2.z = 27.00f;


	/*
	// TEST 03: triangle clamped on one line; For tracing test (same faces)	 (Issue resolved on 7/22/2018)

	testPoint_0.x = -9.0f;
	testPoint_0.y = 1.00f;		// previous: 7.0f
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.0f;
	testPoint_1.y = 1.00f;			// previous: 18.0f
	testPoint_1.z = 1.00f;

	testPoint_2.x = -1.00f;
	testPoint_2.y = 9.00f;			// previous: 10.0f
	testPoint_2.z = 9.00f;
	*/

	/*
	//  TEST 04: reddit reveal
	testPoint_0.x = -26.4f;
	testPoint_0.y = 6.78f;		// previous: 7.0f		// error with 6.78 here (7/5/2018) --> fixed, 7/9/2018
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -10.2f;
	testPoint_1.y = 9.80f;			// previous: 18.0f			(7.80, 10.80, 20.80f); 20.80f leads to interesting resutls/angles
	testPoint_1.z = 4.70f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 7.45f;			// previous: 10.0f
	testPoint_2.z = 27.00f;
	*/


	// REVERSE CLOCKWISE TEST
	/*
	testPoint_0.x = -26.4f;
	testPoint_0.y = 6.78f;		// previous: 7.0f		// error with 6.78 here (7/5/2018) --> fixed, 7/9/2018
	testPoint_0.z = 1.00f;		// previous: 0.0f

	testPoint_1.x = -1.23f;
	testPoint_1.y = 7.45f;			// previous: 10.0f
	testPoint_1.z = 27.00f;

	testPoint_2.x = -10.2f;
	testPoint_2.y = 20.80f;			// previous: 18.0f			(7.80, 10.80, 20.80f); 20.80f leads to interesting resutls/angles
	testPoint_2.z = 4.70f;
	*/

	/*
	// TEST 03: for generating t2 lines from a single t1 line
	testPoint_0.x = -48.0f;
	testPoint_0.y = 7.00f;
	testPoint_0.z = 16.00f;

	testPoint_1.x = 16.0f;
	testPoint_1.y = 7.00f;
	testPoint_1.z = 16.00f;

	testPoint_2.x = -16.0f;
	testPoint_2.y = 7.00f;
	testPoint_2.z = 80.00f;
	*/

	// TEST 04: for testing roundToAppropriatePrecision		(CURRENTLY in  use as of 7/5/2018)

	/*
	testPoint_0.x = -48.0f;		// previous: -48
	testPoint_0.y = 16.00f;
	testPoint_0.z = 16.00f;

	testPoint_1.x = 16.00f;		// previous: 16			// bugged: 16.00f
	testPoint_1.y = 10.00f;								// bugged: 10.00f
	testPoint_1.z = 16.00f;								// bugged: 16.00f

	testPoint_2.x = -16.0f;		// previous: -16
	testPoint_2.y = -80.00f;	// -80.0f = error!
	testPoint_2.z = 112.00f;	// previous: 112, current is 180		 // + 68 *should* cause an error.
	*/


	// TEST 05: right angle with new distance algorithms	-- Seems to be working, 2 and 3 candidate hits are appropriately rounding!
	/*
	testPoint_0.x = -12.0f;
	testPoint_0.y = 10.0f;
	testPoint_0.z = 10.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 2.0f;
	testPoint_2.z = 2.0f;
	*/

	// TEST 06-A: Multi-primary line tracing, segment-end trace

	/**/

	/*
	testPoint_0.x = -42.0f;
	testPoint_0.y = 10.0f;
	testPoint_0.z = 10.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 2.0f;
	testPoint_2.z = 2.0f;
	*/


	// TEST 06-B: Multi-primary line tracing, segment-end trace

	testPoint_0.x = -122.0f;			// test: 82, 150, 122
	testPoint_0.y = 2.4f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short; bug for -2.4 fixed see (10/3/2018)
	testPoint_0.z = 2.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 2.0f;
	testPoint_2.z = 2.0f;



	
	ECBPolyPoint testPoint_4;
	testPoint_4.x = -8.3f;
	testPoint_4.y = 2.0f;
	testPoint_4.z = 20.0f;

	
	/*
	// perfect clamp test
	testPoint_0.x = -42.0f;
	testPoint_0.y = 10.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short; bug for -2.4 fixed see (10/3/2018)
	testPoint_0.z = 2.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 10.0f;
	testPoint_2.z = 2.0f;

	testPoint_4.x = -8.3f;
	testPoint_4.y = 10.0f;
	testPoint_4.z = 20.0f;
	*/
	

	/*
	// TEST 07-a: Multi-primary line tracing, constructing multiple T2 lines from one T1
	testPoint_0.x = -24.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 8.0f;

	testPoint_1.x = 24.0f;
	testPoint_1.y = 12.0f;
	testPoint_1.z = 12.0f;

	testPoint_2.x = 0.0f;
	testPoint_2.y = 8.0f;
	testPoint_2.z = 48.0f;
	*/



	// TEST 07-b: (reverse direction of 07-a)
	/*
	testPoint_0.x = -24.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 8.0f;

	testPoint_1.x = 0.0f;
	testPoint_1.y = 8.0f;
	testPoint_1.z = 48.0f;

	testPoint_2.x = 24.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 12.0f;
	*/



	// TEST 08: Multi-primary line tracing, constructing multiple T2 lines from T1; 3 lines get constructed here
	/*
	testPoint_0.x = -48.0f;
	testPoint_0.y = 10.0f;
	testPoint_0.z = -12.0f;

	testPoint_1.x = -16.0f;
	testPoint_1.y = 20.0f;
	testPoint_1.z = 24.0f;

	testPoint_2.x = 16.0f;
	testPoint_2.y = 10.0f;
	testPoint_2.z = -12.0f;
	*/


	/*
	// test #1 for 1000s range point locations
	testPoint_0.x = 3152.0f;		// previous: -48		// NOW: 3152
	testPoint_0.y = 16.00f;
	testPoint_0.z = 16.00f;

	testPoint_1.x = 3216.0f;		// previous: 16			// NOW: 3216
	testPoint_1.y = 16.00f;
	testPoint_1.z = 16.00f;

	testPoint_2.x = 3184.00f;		// previous: -16		// NOW: 3184	// alternate: 3183.88
	testPoint_2.y = -80.00f;	// -80.0f = error!
	testPoint_2.z = 112.00f;

	// testing for 10000s range point locations; Passed (5/27/2018)
	testPoint_0.x = 31952.0f;		// previous: -48			// NOW: 31952
	testPoint_0.y = 16.00f;
	testPoint_0.z = 16.00f;

	testPoint_1.x = 32016.0f;		// previous: 16				// NOW: 32016
	testPoint_1.y = 16.00f;
	testPoint_1.z = 16.00f;

	testPoint_2.x = 31984.0f;		// previous: -16			// NOW: 31984
	testPoint_2.y = -80.00f;	// -80.0f = error!
	testPoint_2.z = 112.00f;

	// test #2 for 10000s range point locations; Passed (5/27/2018)
	testPoint_0.x = 31952.0f;		// previous: -48			// NOW: 31952
	testPoint_0.y = 32016.00f;			// previous: 16				// NOW: 32016
	testPoint_0.z = 16.00f;

	testPoint_1.x = 32016.0f;		// previous: 16				// NOW: 32016
	testPoint_1.y = 32016.00f;			// previous: 16				// NOW: 32016
	testPoint_1.z = 16.00f;

	testPoint_2.x = 31984.0f;		// previous: -16			// NOW: 31984
	testPoint_2.y = 31920.00f;	// -80.0f = error!				// NOW: 31920
	testPoint_2.z = 112.00f;

	// test #3 for 10000s range point locations;
	testPoint_0.x = 31952.0f;		// previous: -48			// NOW: 31952
	testPoint_0.y = 32016.00f;			// previous: 16				// NOW: 32016
	testPoint_0.z = 32016.00f;				// previous: 16				// NOW: 32016

	testPoint_1.x = 32016.0f;		// previous: 16				// NOW: 32016
	testPoint_1.y = 32016.00f;			// previous: 16				// NOW: 32016
	testPoint_1.z = 32016.00f;				// previous: 16				// NOW: 32016

	testPoint_2.x = 31984.0f;		// previous: -16			// NOW: 31984
	testPoint_2.y = 31920.00f;	// -80.0f = error!				// NOW: 31920
	testPoint_2.z = 32112.00f;		// previous 112.00f			// NOW:								// 32180 causes error!!!!
	*/




	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, 0, 5, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_4, 0, 5, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, 0, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_1, testPoint_0, testPoint_4, 0, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)	// will cause system crash??? (why?)
	//planRef->constructSingleContouredTriangle(testPoint_1, testPoint_0, testPoint_2, 0, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)	// will cause system crash??? (why?)
	executeContourPlan("plan");
}

void OSServer::constructTestBlueprints2() 
{
	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	addContourPlan("plan", OSPDir::BELOW, -85.0f, 80.0f, 90.0f);
	OSContourPlan* planRef = getContourPlan("plan");		// get pointer to the plan

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;
	ECBPolyPoint testPoint_4;
	ECBPolyPoint testPoint_5;
	testPoint_0.x = 16.5f;
	testPoint_0.y = 8.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 20.5f;

	testPoint_1.x = 13.5f;
	testPoint_1.y = 7.0f;
	testPoint_1.z = 26.0f;

	testPoint_2.x = 17.5f;
	testPoint_2.y = 7.0f;
	testPoint_2.z = 26.0f;

	testPoint_3.x = 25.5f;
	testPoint_3.y = 7.0f;
	testPoint_3.z = 17.0f;

	testPoint_4.x = 16.5f;
	testPoint_4.y = 7.0f;
	testPoint_4.z = 15.0f;

	testPoint_5.x = 7.5f;
	testPoint_5.y = 7.0f;
	testPoint_5.z = 15.0f;

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, 0, 10, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, 0, 10, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_3, testPoint_4, 0, 10, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_4, testPoint_5, 0, 10, std::ref(*heapMutexRef));	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_5, testPoint_1, 0, 10, std::ref(*heapMutexRef));
	executeContourPlan("plan");
}

void OSServer::constructTestBlueprints3()
{
	std::cout << "||||||| constructing blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;		
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 2.45;
	mountainSummit.z = 2.61;

	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)


	addContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, 5, 6.81, 9, 9);	// create the points in all contour lines
	OSContourPlan* planRef = getContourPlan("mountain");		// get pointer to the plan
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	std::cout << "Number of contour lines: -->" << planRef->contourLineMap.size() << std::endl;
	OSContourLine* lineRef = &planRef->contourLineMap[0];
	std::cout << "Line ref acquired... >>>" << std::endl;
	std::cout << "Mountain summit is: " << mountainSummit.x << ", " << mountainSummit.y << ", " << mountainSummit.z << std::endl;
	std::cout << "Point 0 is: " << lineRef->smartContourPoint[0].getPolyPoint().x << ", " << lineRef->smartContourPoint[0].getPolyPoint().y << ", " << lineRef->smartContourPoint[0].getPolyPoint().z << std::endl;
	std::cout << "Point 1 is: " << lineRef->smartContourPoint[1].getPolyPoint().x << ", " << lineRef->smartContourPoint[1].getPolyPoint().y << ", " << lineRef->smartContourPoint[1].getPolyPoint().z << std::endl;
	std::cout << "Point 2 is: " << lineRef->smartContourPoint[2].getPolyPoint().x << ", " << lineRef->smartContourPoint[2].getPolyPoint().y << ", " << lineRef->smartContourPoint[2].getPolyPoint().z << std::endl;
	std::cout << "Point 3 is: " << lineRef->smartContourPoint[3].getPolyPoint().x << ", " << lineRef->smartContourPoint[3].getPolyPoint().y << ", " << lineRef->smartContourPoint[3].getPolyPoint().z << std::endl;

	// for summit at 30, 5, 5
	/*
	ECBPolyPoint otherPoint0;
	otherPoint0.x = 14;
	otherPoint0.y = 20;
	otherPoint0.z = 5;

	ECBPolyPoint otherPoint1;
	otherPoint1.x = 5;
	otherPoint1.y = 20;
	otherPoint1.z = 14;

	ECBPolyPoint otherPoint2;
	otherPoint2.x = -4;
	otherPoint2.y = 20;
	otherPoint2.z = 5;

	ECBPolyPoint otherPoint3;
	otherPoint3.x = 5;
	otherPoint3.y = 20;
	otherPoint3.z = -4;
	*/
	//planRef->constructSingleContouredTriangle(mountainSummit, otherPoint0, otherPoint1, 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, otherPoint1, otherPoint2, 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, otherPoint2, otherPoint3, 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, otherPoint3, otherPoint0, 0, 10, std::ref(*heapMutexRef));
	//planRef->buildTriangleStrips(0);

	// length of 13, paired with a depth between lines of 10.81 causes uncoordinated triangle

	planRef->constructStripTriangles(0, 10, std::ref(*heapMutexRef));		// new function: produces all triangles in a strip, when points are ready etc
	planRef->constructStripTriangles(1, 10, std::ref(*heapMutexRef));
	planRef->constructStripTriangles(2, 10, std::ref(*heapMutexRef));
	planRef->constructStripTriangles(3, 10, std::ref(*heapMutexRef));
	planRef->constructStripTriangles(4, 10, std::ref(*heapMutexRef));
	//planRef->constructStripTriangles(5, 10, std::ref(*heapMutexRef));
	//planRef->constructStripTriangles(6, 10, std::ref(*heapMutexRef));

	//planRef->constructSingleContouredTriangle(mountainSummit, lineRef->smartContourPoint[0].getPolyPoint(), lineRef->smartContourPoint[1].getPolyPoint(), 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, lineRef->smartContourPoint[1].getPolyPoint(), lineRef->smartContourPoint[2].getPolyPoint(), 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, lineRef->smartContourPoint[2].getPolyPoint(), lineRef->smartContourPoint[3].getPolyPoint(), 0, 10, std::ref(*heapMutexRef));
	//planRef->constructSingleContouredTriangle(mountainSummit, lineRef->smartContourPoint[3].getPolyPoint(), lineRef->smartContourPoint[0].getPolyPoint(), 0, 10, std::ref(*heapMutexRef));

	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeContourPlan("mountain");
}

void OSServer::constructTestBlueprints4()
{
	std::cout << "||||||| constructing blueprints (version 4)...." << std::endl;
	ECBPolyPoint point_0;
	point_0.x = -16;
	point_0.y = 32;
	point_0.z = 16;

	ECBPolyPoint point_1;
	point_1.x = 48;
	point_1.y = 16;
	point_1.z = 16;

	ECBPolyPoint point_2;
	point_2.x = -16;
	point_2.y = 16;
	point_2.z = 32;

	/*
		ECBPolyPoint point_0;
	point_0.x = -16;
	point_0.y = 32;
	point_0.z = 16.23;

	ECBPolyPoint point_1;
	point_1.x = 48;
	point_1.y = 16;
	point_1.z = 16.87;

	ECBPolyPoint point_2;
	point_2.x = -16;
	point_2.y = 16;
	point_2.z = 32.37;
	*/

	addContourPlan("plan", OSPDir::BELOW, -85.0f, 80.0f, 90.0f);
	OSContourPlan* planRef = getContourPlan("plan");		// get pointer to the plan
	planRef->constructSingleContouredTriangle(point_0, point_1, point_2, 0, 5, std::ref(*heapMutexRef));
	executeContourPlan("plan");

}

void OSServer::executeContourPlan(string in_string)
{
	OSContourPlan* planPtr = &contourPlanMap[in_string];
	int numberOfTriangleStrips = planPtr->triangleStripMap.size();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapIterator = planPtr->triangleStripMap.begin();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapEnd = planPtr->triangleStripMap.end();
	
	for (stripMapIterator; stripMapIterator != stripMapEnd; stripMapIterator++)
	{
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = stripMapIterator->second.triangleMap.end();
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			//cout << "Current triangle ID: " << triangleMapIterator->first << endl;
			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections);

			// write (overwrite) a blueprint file for each blueprint traversed
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorBegin = currentTriangle->polygonPieceMap.begin();
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorEnd = currentTriangle->polygonPieceMap.end();
			for (keyIteratorBegin; keyIteratorBegin != keyIteratorEnd; keyIteratorBegin++)
			{
				EnclaveKeyDef::EnclaveKey currentFileName = keyIteratorBegin->first;	// get the blueprint traversed
				std::cout << ">> Blueprint file to write is: " << currentFileName.x << ", " << currentFileName.y << ", " << currentFileName.z << ", " << std::endl;
				EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[currentFileName];
				OSWinAdapter::writeBlueprintPolysToFile(currentWorld, currentFileName, blueprintRef);
				EnclaveCollectionBlueprint readBackBP;
				OSWinAdapter::readBlueprintPolysFromFile(currentWorld, currentFileName, &readBackBP);
				OSWinAdapter::outputBlueprintStats(&readBackBP);
			}

		}
	}
}

void OSServer::transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	organicSystemPtr->AddBlueprint(in_key, &blueprintMap[in_key], 0);		// assign constructed blueprint to organic system
}

void OSServer::sendAndRenderBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	transferBlueprintToLocalOS(in_key);
	organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(in_key);
	organicSystemPtr->AddKeyToRenderList(in_key);
}

void OSServer::sendAndRenderAllBlueprintsToLocalOS()
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintBegin = blueprintMap.begin();
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintEnd = blueprintMap.end();
	for (blueprintBegin; blueprintBegin != blueprintEnd; blueprintBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentKey = blueprintBegin->first;
		transferBlueprintToLocalOS(currentKey);
		organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(currentKey);
		organicSystemPtr->AddKeyToRenderList(currentKey);
	}
}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	calibrateAndRunContouredTriangle(in_Triangle, in_Directions);		// perform calibrations on this single contoured triangle, so that points of the triangle are in the appropriate EnclaveKey
	//determineTriangleType2and3Lines(in_Triangle);		// T-4 cycle through triangle border polys
}

void OSServer::determineTriangleCentroid(OSContouredTriangle* in_Triangle)
{
	// reserved for later, potentially will be axed/deleted
}


void OSServer::determineTriangleType2and3Lines(OSContouredTriangle* in_Triangle)
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyIterBegin = in_Triangle->polygonPieceMap.begin();
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyIterEnd = in_Triangle->polygonPieceMap.end();
	for (polyIterBegin; polyIterBegin != polyIterEnd; polyIterBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentBlueprintKey = polyIterBegin->first;
		int currentBlueprintPoly = polyIterBegin->second;
		ECBPoly* polyPtr = &blueprintMap[currentBlueprintKey].primaryPolygonMap[currentBlueprintPoly];
		std::unordered_map<int, ECBPolyLine>::iterator polyLineIterBegin = polyPtr->lineMap.begin();
		std::unordered_map<int, ECBPolyLine>::iterator polyLineIterEnd = polyPtr->lineMap.end();
		//cout << "--------Key: " << currentBlueprintKey.x << ", " << currentBlueprintKey.y << ", " << currentBlueprintKey.z << endl;

		for (polyLineIterBegin; polyLineIterBegin != polyLineIterEnd; polyLineIterBegin++)
		{
			//cout << "Line detected!!! " << endl;
			//cout << "Point A: " << polyLineIterBegin->second.pointA.x << ", " << polyLineIterBegin->second.pointA.y << ", " << polyLineIterBegin->second.pointA.z << endl;
			//cout << "Point B: " << polyLineIterBegin->second.pointB.x << ", " << polyLineIterBegin->second.pointB.y << ", " << polyLineIterBegin->second.pointB.z << endl;
		}
		//cout << "----------------" << endl;
	}

	EnclaveKeyDef::EnclaveKey debugKey;
	debugKey.x = -1;
	std::cout << "(DEBUG ATTEMPT 2) Current poly line count is: " << blueprintMap[debugKey].primaryPolygonMap[0].lineMap.size() << std::endl;
}

void OSServer::writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys)
{
	EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[in_keys];	// get a pointer to the blueprint
	std::unordered_map<int, ECBPoly>::iterator currentPrimaryPoly = blueprintRef->primaryPolygonMap.begin();	// beginning of the poly map
	std::unordered_map<int, ECBPoly>::iterator primaryPolyEnd = blueprintRef->primaryPolygonMap.end();		// ending of the poly map
	for (currentPrimaryPoly; currentPrimaryPoly != primaryPolyEnd; ++currentPrimaryPoly)
	{

	}
}

void OSServer::analyzeECBPoly(ECBPoly* in_polyRef)
{

}

void OSServer::setCurrentWorld(std::string in_worldName)
{
	currentWorld = in_worldName;
}

void OSServer::calibrateAndRunContouredTriangle(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	cout << "Relativity job BEGIN ||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
	// step 1: check for Type 1 condition: 2 points of triangle that have a pair of the same coordinates (clamped to an axis)
	int conditionMetFlag = 0;			// determines what condition has been met

	// used for this function only
	struct lineMeta
	{
		int line_ID = 0;				// for the loop
		OSTriangleLine* structLinePtr;	// ptr to the actual line
		ECBPolyPoint thirdPoint;		// = to point B of next line in the sequence
		int is_clamped = 0;				// is this line clamped?
	};

	lineMeta tempMeta[3];				// set up the struct

	for (int x = 0; x < 3; x++)
	{
		tempMeta[x].line_ID = x;
		tempMeta[x].structLinePtr = &in_Triangle->triangleLines[x];	
		if (x < 2)
		{
			tempMeta[x].thirdPoint = in_Triangle->triangleLines[x + 1].pointB;		// store the next point in the sequence
		}
		else if (x == 2)
		{
			tempMeta[x].thirdPoint = in_Triangle->triangleLines[0].pointB;		// store the first point in the sequence
		}



		OSTriangleLine* linePtr = &in_Triangle->triangleLines[x];	// get reference to appropriate line
		int x_match = 0;					// indicates B.x- A.x = 0
		int y_match = 0;					// y
		int z_match = 0;					// z
		int conditionCount = 0;				// if this becomes 2, it is a Type 1 


		//  x coord check
		if ((linePtr->pointB.x - linePtr->pointA.x) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			cout << "X check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_x = 1;
			if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) != 0)
			{
				cout << "X shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) > 0)
				{
					cout << "X difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_x = 1;
					tempMeta[x].structLinePtr->x_clamp_direction = 1;
				}
				else if ((tempMeta[x].thirdPoint.x - tempMeta[x].structLinePtr->pointA.x) < 0)
				{
					cout << "X difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_x = 1;
					tempMeta[x].structLinePtr->x_clamp_direction = -1;
				}
			}
			x_match = 1;
		}

		// y
		if ((linePtr->pointB.y - linePtr->pointA.y) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			cout << "Y check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_y = 1;			// indicate the line is clamped to y
			if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) != 0)
			{
				cout << "Y shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) > 0)
				{
					cout << "Y difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_y = 1;				// signal this line is clamped to y
					tempMeta[x].structLinePtr->y_clamp_direction = 1;			// positive slope = positive y
				}
				else if ((tempMeta[x].thirdPoint.y - tempMeta[x].structLinePtr->pointA.y) < 0)
				{
					cout << "Y difference is negative" << endl;
					tempMeta[x].structLinePtr->clamped_to_y = 1;				
					tempMeta[x].structLinePtr->y_clamp_direction = -1;			
				}
			}
			y_match = 1;
		}

		// z
		if ((linePtr->pointB.z - linePtr->pointA.z) == 0)	// if they are equal to exactly 0, increment
		{
			conditionCount++;
			//cout << "Z check: " << endl;
			tempMeta[x].structLinePtr->clamped_to_z = 1;
			if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) != 0)
			{
				//cout << "Z shift is not 0" << endl;
				if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) > 0)
				{
					//cout << "Z difference is positive" << endl;
					tempMeta[x].structLinePtr->clamped_to_z = 1;
					tempMeta[x].structLinePtr->z_clamp_direction = 1;
				}
				else if ((tempMeta[x].thirdPoint.z - tempMeta[x].structLinePtr->pointA.z) < 0)
				{
					//cout << "Z difference is negative" << endl;
					tempMeta[x].structLinePtr->clamped_to_z = 1;
					tempMeta[x].structLinePtr->z_clamp_direction = -1;
				}
			}
			z_match = 1;
		}
		
		/*
		if (conditionCount == 2)
		{
			cout << "TYPE 1 condition met. " << endl;
			// other new stuff
			if (y_match == 1 && z_match == 1)	// triangle pivots on x, perform checks
			{
				cout << "TYPE 1: pivots on X axis" << endl;
				// get y values of all points...check to see if its perfectly flat.
				if (
					(in_Triangle->trianglePoints[0].y == in_Triangle->trianglePoints[1].y)
					&&
					(in_Triangle->trianglePoints[1].y == in_Triangle->trianglePoints[2].y)
				   )
				{
					in_Triangle->isTriangleFlat = 1; // make sure to establish that its officially flat
					cout << "TYPE 1: x-pivot is perfectly flat...no angle!! " << endl;
				}

			}
			else if (x_match == 1 && y_match == 1)	// .... on y
			{
				cout << "TYPE 1: pivots on Z axis" << endl;
				if (
					(in_Triangle->trianglePoints[0].y == in_Triangle->trianglePoints[1].y)
					&&
					(in_Triangle->trianglePoints[1].y == in_Triangle->trianglePoints[2].y)
					)
				{
					in_Triangle->isTriangleFlat = 1; // make sure to establish that its officially flat
					cout << "TYPE 1: z-pivot is perfectly flat...no angle!! " << endl;
				}
			}
			else // .... otherwise, it's z
			
			{
				cout << "TYPE 1: pivots on Y axis" << endl;
				// no y-flat checks need to be done (because this is default elevation mode)
			}

			conditionMetFlag = 1;	// indicates condition 1 was met
			break;					// exit when 2 (condition 1 is met)
		}
		else if (conditionCount == 1)
		{

		*/
	}

	// Step 2: check if triangle is perfectly clamped to x, y, or z (Type 2 condition)
	if ((in_Triangle->triangleLines[0].clamped_to_x == 1) && (in_Triangle->triangleLines[1].clamped_to_x == 1) && (in_Triangle->triangleLines[2].clamped_to_x == 1))
	{
		cout << "triangle is perfectly clamped to x!" << endl;
		in_Triangle->perfect_clamp_x = 1;
	}
	if ((in_Triangle->triangleLines[0].clamped_to_y == 1) && (in_Triangle->triangleLines[1].clamped_to_y == 1) && (in_Triangle->triangleLines[2].clamped_to_y == 1))
	{
		cout << "triangle is perfectly clamped to y!" << endl;
		in_Triangle->perfect_clamp_y = 1;
	}
	if ((in_Triangle->triangleLines[0].clamped_to_z == 1) && (in_Triangle->triangleLines[1].clamped_to_z == 1) && (in_Triangle->triangleLines[2].clamped_to_z == 1))
	{
		cout << "triangle is perfectly clamped to z!" << endl;
		in_Triangle->perfect_clamp_z = 1;
	}


	// step 3: calibrate point keys, after conditions have been checked

	// STEP T-2
	calibrateTrianglePointKeys(in_Triangle, in_Directions);


	// STEP T-3
	rayCastTrianglePoints(in_Triangle);		// runs the lines through the "world" and sets them up in their appropriate blueprints


	// STEP T-4
	auto blueend = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < 3; x++)
	{
		//std::cout << "Line " << x << std::endl;
		std::cout << in_Triangle->triangleLines[x].pointA.x << ", " << in_Triangle->triangleLines[x].pointA.y << ", " << in_Triangle->triangleLines[x].pointA.z << std::endl;
		std::cout << in_Triangle->triangleLines[x].pointB.x << ", " << in_Triangle->triangleLines[x].pointB.y << ", " << in_Triangle->triangleLines[x].pointB.z << std::endl;
	}
}

void OSServer::rayCastTrianglePoints(OSContouredTriangle* in_Triangle)
{
	// loop through each point
	for (int x = 0; x < 3; x++)
	{
		//std::cout << "~~~~~~~~~~~~~~Beginning trace of line " << x <<  std::endl;
		//cout << "Key value of point:  " << in_Triangle->pointKeys[x].x << ", " << in_Triangle->pointKeys[x].y << ", " << in_Triangle->pointKeys[x].z << endl;
		int checkResult = checkIfBlueprintExists(in_Triangle->pointKeys[x]);	// returns 0 if it didn't exist, 1 if so
		if (checkResult == 0)		// create a blueprint if it doesnt exist already
		{
			// STEP T-3.2
			tracePointThroughBlueprints(in_Triangle, x);					// trace point (line) through blueprints
			//cout << "Blueprint didn't exist for polygon insertion, creating with polygonPieceMap insert at element 0" << endl;
		}
		else if (checkResult == 1)	// blueprint already exists
		{
			// STEP T-3.2
			tracePointThroughBlueprints(in_Triangle, x);
			/*
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(in_Triangle->pointKeys[x]);	// check to see if the polygon exists already in the contoured triangle
			if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
			{
				int blueprintIDofFoundPolygon = polyMapIter->second;	// get the corresponding int value from the triangle's registered blueprint polygon map
				EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[in_Triangle->pointKeys[x]];	// get a pointer to the blueprint (for code readability only)
				//ECBPolyLine newPolyLine;
				//newPolyLine.pointA = in_Triangle->triangleLines[x].pointA;		// set the new line to the pointed-to point A
				//newPolyLine.pointB = in_Triangle->triangleLines[x].pointB;		// set the new line to the pointed-to point B
				// need angle set here
				// need angle set here
				//blueprintPtr->primaryPolygonMap[blueprintIDofFoundPolygon].lineMap[x] = newPolyLine;
				cout << "Blueprint existed for polygon insertion. " << endl;
			}
			else  // wasn't found
			{

			}
			*/
		}
		//std::cout << "~~~~~~~~~~~~~~Ending trace of line " << x << std::endl;
	}
}

void OSServer::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID)
{
	in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_Triangle->triangleLines[in_pointID].pointA;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_Triangle->triangleLines[in_pointID].pointB;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z
}

void OSServer::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint)
{
	in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_beginPoint;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_endPoint;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z
}

void OSServer::runServer()
{
	if (serverRunMode == 0)				// server would be alone, with no instance of an OrganicSystem (prototype only, uncertain of use)
	{
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0)	// run until the command line is shutdown
		{

		}
	}
	else if (serverRunMode == 1)		// server runs actively alongside an instance of an OrganicSystem
	{
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0) // ""
		{
			organicSystemPtr->runOrganicTick();
		}
		organicSystemPtr->GLCleanup();
	}
}

void OSServer::executeCommandLine()
{
	int* commandLineRunningRef = &isCommandLineRunning;
	int* clShutdownStatus = &isCommandLineShutDown;
	std::future<int> testFuture2 = OSCManager.stemcellMap[0].threadPtr->submit(&OSServer::runCommandLine, this, std::ref(serverReadWrite), std::ref(commandLineCV), std::ref(isCommandLineRunning), std::ref(clShutdownStatus));
}

int OSServer::runCommandLine(mutex& in_serverReadWrite, std::condition_variable& in_conditionVariable, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus)
{
	std::cout << ">> Job submission begin..." << std::endl;
	int* isCommandLineRunningPtr = &in_commandLineRunningStatus;
	while (*isCommandLineRunningPtr == 1)
	{
		std::cout << "Waiting for command...." << std::endl;
		int input_value;
		std::cout << "> ";
		std::cin >> input_value;
		setServerStatus(std::ref(in_serverReadWrite), input_value, std::ref(isCommandLineRunningPtr));
	}
	signalServerShutdown(std::ref(in_serverReadWrite));
	signalCommandLineShutdown(std::ref(in_serverReadWrite), 1, std::ref(is_commandLineShutDownStatus));
	int returnVal = 5;
	return returnVal;
}

int OSServer::checkServerStatus(mutex& in_serverReadWrite)
{
	std::lock_guard<std::mutex> lock(std::ref(serverReadWrite));
	return isServerActive;
}


void OSServer::setServerStatus(mutex& in_serverReadWrite, int in_valueToSet, int* in_commandLineStatus)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	if (in_valueToSet == 0)
	{
		//isServerActive = in_valueToSet;
		std::cout << "shutdown flag found! " << std::endl;
		*in_commandLineStatus = 0;
	}
	else if (in_valueToSet == 10)	// "stop" the test thread"
	{
		//threadController.signalStop();
		OSCManager.stemcellMap[1].signalStop();		// halt the test thread
	}
	else if (in_valueToSet == 11)
	{
		//threadController.signalStart();
		OSCManager.stemcellMap[1].signalStart();	// resume the test thread
	}
	else if (in_valueToSet == 12)
	{
		std::cout << "Attempting delete....>>" << std::endl;
		//.deleteThread();
		std::cout << "Delete succeeded..." << std::endl;
	}

	else if (in_valueToSet == 5)
	{
		std::cout << "Toggling targeting..." << std::endl;
		organicSystemPtr->director.toggleTargeting();
	}
}

void OSServer::signalCommandLineShutdown(mutex& in_serverReadWrite, int in_valueToSet, int* in_clShutdownFlag)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	std::cout << "(1) shutdown flag call complete" << std::endl;
	//*in_clShutdownFlag = 1;
	isCommandLineShutDown = 1;
	std::cout << "(2) shutdown flag call complete" << std::endl;
}

int OSServer::getCommandLineShutdownValue(mutex& in_serverReadWrite)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	return isCommandLineShutDown;
}

void OSServer::signalServerShutdown(mutex& in_serverMutex)
{
	std::lock_guard<std::mutex> lock(std::ref(in_serverMutex));
	isServerActive = 0;
}

void OSServer::tracePointThroughBlueprints(OSContouredTriangle* in_Triangle, int in_pointID)
{
	std::lock_guard<std::mutex> lock(*heapMutexRef);
	// STEP 1:	set appropriate values of keys 
	EnclaveKeyDef::EnclaveKey originPointKey;
	EnclaveKeyDef::EnclaveKey endPointKey;

	if (in_pointID < 2)
	{
		originPointKey = in_Triangle->pointKeys[in_pointID];
		endPointKey = in_Triangle->pointKeys[in_pointID + 1];
	}
	else if (in_pointID == 2)
	{
		originPointKey = in_Triangle->pointKeys[2];
		endPointKey = in_Triangle->pointKeys[0];
	}
	EnclaveKeyDef::EnclaveKey incrementingKey = originPointKey;		// will constantly increment and/or decrement as it traverses blueprints

	// STEP 2: initiating tracing
	if (originPointKey == endPointKey)		// both points exist in same blueprint
	{
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
		{
			
			int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];	// get a pointer to the blueprint (for code readability only)
			ECBPolyLine newPolyLine;												// create a new poly line
			fillLineMetaData(&newPolyLine, in_Triangle, in_pointID);
			blueprintPtr->primaryPolygonMap[polygonIDinBlueprint].lineMap[in_pointID] = newPolyLine;
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
		}
		else  // polygon wasn't found, it needs to be created
		{
			
			ECBPoly newPoly;
			newPoly.materialID = in_Triangle->materialID;
			fillPolyWithClampResult(&newPoly, in_Triangle);
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];
			int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
			blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
			ECBPolyLine newPolyLine;												// create a new poly line
			fillLineMetaData(&newPolyLine, in_Triangle, in_pointID);
			blueprintPtr->primaryPolygonMap[elementID].lineMap[in_pointID] = newPolyLine;
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
			in_Triangle->addPolygonPiece(incrementingKey, elementID);
			
		}
		
	}
	else								// points do not exist in same blueprint
	{
		//cout << "The begin point for line " << in_pointID << " is NOT in same area as its endpoint" << endl;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_Triangle->polygonPieceMap.find(incrementingKey);	// check to see if the polygon exists already in the contoured triangle
		if (polyMapIter != in_Triangle->polygonPieceMap.end())	// polygon was already found
		{
			int blueprintIDofFoundPolygon = polyMapIter->second;											// get the corresponding int value from the triangle's registered blueprint polygon map
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];	// get a pointer to the blueprint (for code readability only)
			OSTriangleLineTraverser lineTraverser(in_Triangle, in_pointID, this);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				lineRef->traverseLineOnce(in_Triangle);
			}
			

			
		}
		else  // polygon wasn't found, it needs to be created
		{
			ECBPoly newPoly;
			newPoly.materialID = in_Triangle->materialID;
			fillPolyWithClampResult(&newPoly, in_Triangle);
			EnclaveCollectionBlueprint* blueprintPtr = &blueprintMap[incrementingKey];
			int currentBlueprintPolyMapSize = blueprintPtr->primaryPolygonMap.size();
			blueprintPtr->primaryPolygonMap[currentBlueprintPolyMapSize] = newPoly;		// insert a new polygon; the ID will be equalto the size
			//OSWinAdapter::writeBlueprintPolysToFile(incrementingKey, &blueprintMap);
			in_Triangle->addPolygonPiece(incrementingKey, currentBlueprintPolyMapSize);

			OSTriangleLineTraverser lineTraverser(in_Triangle, in_pointID, this);
			OSTriangleLineTraverser* lineRef = &lineTraverser;
			while (!(lineRef->currentKey == lineRef->endKey))
			{
				lineRef->traverseLineOnce(in_Triangle);
			}
			
		}
	}
}

void OSServer::fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle)
{
	if (in_contouredTriangle->perfect_clamp_x == 1
		||
		in_contouredTriangle->perfect_clamp_y == 1
		||
		in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 1;
	}

	if (in_contouredTriangle->perfect_clamp_x == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 1;
	}
	if (in_contouredTriangle->perfect_clamp_y == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 2;
	}
	if (in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 3;
	}
}

void OSServer::calibrateTrianglePointKeys(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions)
{
	EnclaveKeyDef::EnclaveKey currentKeyCopy;
	ECBBorderLineList currentBorderLineList;
	for (int x = 0; x < 3; x++)
	{
		OSTriangleLine currentLine = in_Triangle->triangleLines[x];											// get the line
		EnclaveKeyDef::EnclaveKey* currentKeyPtr = &in_Triangle->pointKeys[x];									// get a pointer to the key of the point
		currentKeyCopy = in_Triangle->pointKeys[x];									// get a copy to the key of the point, to determine the original ECBBorderLineList from the pre-modified EnclaveKey of the point
		currentBorderLineList = OrganicUtils::determineBorderLines(currentKeyCopy);			// get the ecb border line list	
		findTrueKeysForTriangleLinePoints(in_Triangle, currentLine, currentKeyPtr, currentBorderLineList);	// calculate the true key for the points in the lines. This function call handles one point of the contoured triangle per call. (so loop 3 times)
	}

	// check for perfect clamps; we can use the last iteration of currentKeyCopy for this
	if (in_Triangle->perfect_clamp_x == 1)
	{
		/*
		
		The below logic is needed, because of the 32 = next rule in the blueprint system; For example, if the x of the points in question is 32, then 
		this lies on the border of the blueprints at 0,0,0 and 1,0,0. If it's perfectly flat, we must check the direction of x that the center of the contour line lies in.
		
		*/

		OSTriangleLine tempLine = in_Triangle->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.x == currentBorderLineList.corner_LowerNW.cornerPoint.x)		// check the triangle centroid, compare it to the center of the contour line 
		{
			if (in_Directions.x_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].x -= 1;
					cout << "Key altered as a result of perfect clamping (X NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.x == currentBorderLineList.corner_LowerNE.cornerPoint.x)
		{
			if (in_Directions.x_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].x += 1;
					cout << "Key altered as a result of perfect clamping (X POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

	else if (in_Triangle->perfect_clamp_y == 1)
	{
		OSTriangleLine tempLine = in_Triangle->triangleLines[0];	// when checking for any x,y,z or that is clamped, we can get any point in any line (x, y, or z will be the same in all points)
		if (tempLine.pointA.y == currentBorderLineList.corner_LowerNW.cornerPoint.y)		// triangle is at very bottom
		{
			if (in_Directions.y_direction == -1)	// if the clamped triangle is at the very bottom and plan direction is BELOW, shift all points by -1
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].y -= 1;
					cout << "Key altered as a result of perfect clamping (Y NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.y == currentBorderLineList.corner_UpperNW.cornerPoint.y)	// triangle is at very top
		{
			if (in_Directions.y_direction == 1)		// if the clamped triangle is at the very top and plan direction is ABOVE, shift all points by 1
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].y += 1;
					cout << "Key altered as a result of perfect clamping (Y POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

	else if (in_Triangle->perfect_clamp_z == 1)
	{
		OSTriangleLine tempLine = in_Triangle->triangleLines[0];
		if (tempLine.pointA.z == currentBorderLineList.corner_LowerNW.cornerPoint.z)		// triangle is at very bottom
		{
			if (in_Directions.z_direction == -1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].z -= 1;
					cout << "Key altered as a result of perfect clamping (Z NEGATIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
		else if (tempLine.pointA.z == currentBorderLineList.corner_LowerSW.cornerPoint.z)
		{
			if (in_Directions.z_direction == 1)
			{
				for (int a = 0; a < 3; a++)
				{
					in_Triangle->pointKeys[a].z += 1;
					cout << "Key altered as a result of perfect clamping (Z POSITIVE): " << in_Triangle->pointKeys[a].x << ", " << in_Triangle->pointKeys[a].y << ", " << in_Triangle->pointKeys[a].z << endl;
				}
			}
		}
	}

}

void OSServer::findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, OSTriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList)
{
	EnclaveKeyDef::EnclaveKey calibratedKey;
	//EnclaveKeyDef::EnclaveKey curentKey = *in_KeyPtr;
	//EnclaveKeyDef::EnclaveKey keyForBlueprintBorders = *in_KeyPtr;
	//ECBBorderValues borderValues = OrganicUtils::getBlueprintLimits(keyForBlueprintBorders);


	ECBPolyPoint pointToCheck = in_Line.pointA;
	std::cout << "OrganicServerLib, pointToCheck: " << pointToCheck.x << ", " << pointToCheck.y << ", " << pointToCheck.z << ", " << std::endl;
	// NEW CODE BELOW |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// First, cycle through all 8 corner points: ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	if (pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x	&&		pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y	&&		pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)		// Lower NW
	{
		//cout << "Point is at lower NW..." << endl;
		//ECBPolyPoint passPoint = in_borderLineList.corner_LowerNW.cornerPoint;
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);

	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerSW.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)			// Lower SW
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower SW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperSW.cornerPoint.x   &&	pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y   &&  pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)			// Upper SW	 
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperSW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper SW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperNW.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)			// Upper NW
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperNW.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper NW
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x	&&  pointToCheck.y == in_borderLineList.corner_LowerNE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerNE.cornerPoint.z)			// Lower NE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerNE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower NE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_LowerSE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_LowerSE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_LowerSE.cornerPoint.z)			// Lower SE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_LowerSE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, lower SE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperSE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperSE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperSE.cornerPoint.z)			// Upper SE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperSE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper SE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else if (pointToCheck.x == in_borderLineList.corner_UpperNE.cornerPoint.x	&&	pointToCheck.y == in_borderLineList.corner_UpperNE.cornerPoint.y	&&	pointToCheck.z == in_borderLineList.corner_UpperNE.cornerPoint.z)			// Upper NE
	{
		EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.corner_UpperNE.cornerAmpValues, in_Line.pointA, in_Line.pointB);	// get the shifting key, upper NE
		*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
	}
	else
	{
		
		// Second, cycle through all lines	||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

		// start with x axis lines:			

		// Lower North line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if ((pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_LowerNW.cornerPoint.x	&&		pointToCheck.x < in_borderLineList.corner_LowerNE.cornerPoint.x)	// is x between the LowerNW and LowerNE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_lowerNorth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Lower South  line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_LowerSW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_LowerSW.cornerPoint.x	&&	pointToCheck.x < in_borderLineList.corner_LowerSE.cornerPoint.x)		// is x between the LowerSW and LowerSE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_lowerSouth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper South line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_UpperSW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_UpperSW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_UpperSW.cornerPoint.x	&& pointToCheck.x < in_borderLineList.corner_UpperSE.cornerPoint.x)			// is x between the UpperSW and UpperSE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_upperSouth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper North line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			&&
			(pointToCheck.z == in_borderLineList.corner_UpperNW.cornerPoint.z)
			&&
			(pointToCheck.x > in_borderLineList.corner_UpperNW.cornerPoint.x	&& pointToCheck.x < in_borderLineList.corner_UpperNE.cornerPoint.x)			// is x between the UpperNW and UpperNE corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Xaxis_upperNorth, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
	    


		// now do z lines:

		// Lower West line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_LowerNW.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_LowerSW.cornerPoint.z)			// is z between the LowerNW and LowerSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_lowerWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper West line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_UpperNW.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_UpperNW.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_UpperSW.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_upperWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// Upper East line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_UpperNE.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_UpperNE.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_UpperNE.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_UpperSE.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_upperEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// Lower East line check-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			&&
			(pointToCheck.y == in_borderLineList.corner_LowerNE.cornerPoint.y)
			&&
			(pointToCheck.z > in_borderLineList.corner_LowerNE.cornerPoint.z	&& pointToCheck.z < in_borderLineList.corner_LowerSE.cornerPoint.z)			// is z between the UpperNW and UpperSW corners, but not equal to either of them?
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Zaxis_lowerEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// now do y lines:

		// North East line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNE.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerNE.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperNE.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_northEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}

		// South East line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerSE.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSE.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerSE.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperSE.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_southEast, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// South West line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerSW.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerSW.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperSW.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_southWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		// North West line
		else if ((pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			&&
			(pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			&&
			(pointToCheck.y > in_borderLineList.corner_LowerNW.cornerPoint.y	&& pointToCheck.y < in_borderLineList.corner_UpperNW.cornerPoint.y)
			)
		{
			EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.Yaxis_northWest, in_Line.pointA, in_Line.pointB);	// get the shifting key
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
		}
		else
		{
			// Third, cycle through all faces ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

			// West face check
			if (pointToCheck.x == in_borderLineList.corner_LowerNW.cornerPoint.x)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.WestFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// North face check
			else if (pointToCheck.z == in_borderLineList.corner_LowerNW.cornerPoint.z)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.NorthFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// East face check
			else if (pointToCheck.x == in_borderLineList.corner_LowerNE.cornerPoint.x)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.EastFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// South face check
			else if (pointToCheck.z == in_borderLineList.corner_LowerSW.cornerPoint.z)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.SouthFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// Top face check
			else if (pointToCheck.y == in_borderLineList.corner_UpperNW.cornerPoint.y)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.TopFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			// Bottom face check
			else if (pointToCheck.y == in_borderLineList.corner_LowerNW.cornerPoint.y)
			{
				EnclaveKeyDef::EnclaveKey newKey = OrganicUtils::getBorderShiftResult(in_borderLineList.BottomFace, in_Line.pointA, in_Line.pointB);	// get the shifting key
				*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, newKey);
			}
			
		}
		
	}

	// check for influence from clamp flags in the line; if its clamped to x/y/z, check to see if it rotates on this line's axis in either negative or positive fashion (needs explanation/diagram)
	if ((in_Line.clamped_to_x == 1) && (in_Triangle->perfect_clamp_x == 0))
	{
		cout << "x clamp entry" << endl;
		if ((in_Line.x_clamp_direction == 1)	&&	(in_Line.pointA.x == in_borderLineList.corner_LowerNE.cornerPoint.x))	
		{
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.x_clamp_direction == -1) && (in_Line.pointA.x == in_borderLineList.corner_LowerNW.cornerPoint.x)) 
		{
			cout << "negative entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.x = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}
	if ((in_Line.clamped_to_y == 1) && (in_Triangle->perfect_clamp_y == 0))
	{
		cout << "y clamp entry" << endl;
		if ((in_Line.y_clamp_direction == 1) && (in_Line.pointA.y == in_borderLineList.corner_UpperNW.cornerPoint.y))		
		{
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.y = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.y_clamp_direction == -1) && (in_Line.pointA.y == in_borderLineList.corner_LowerNW.cornerPoint.y))	
		{
			cout << "negative entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.y = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}
	if ((in_Line.clamped_to_z == 1) && (in_Triangle->perfect_clamp_z == 0))
	{
		//cout << "z clamp entry" << endl;
		if ((in_Line.z_clamp_direction == 1) && (in_Line.pointA.z == in_borderLineList.corner_UpperSW.cornerPoint.z))
		{
			cout << "positive entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.z = 1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
		else if ((in_Line.z_clamp_direction == -1) && (in_Line.pointA.z == in_borderLineList.corner_UpperNW.cornerPoint.z))
		{
			cout << "negative entry " << endl;
			EnclaveKeyDef::EnclaveKey tempKey;
			tempKey.z = -1;
			*in_KeyPtr = OrganicUtils::addEnclaveKeys(*in_KeyPtr, tempKey);
		}
	}

	//cout << "Pointed to key, post-calc::: " << endl;
	//cout << "x: " << in_KeyPtr->x << endl;
	//cout << "y: " << in_KeyPtr->y << endl;
	//cout << "z: " << in_KeyPtr->z << endl;

	//return calibratedKey;
}


int OSServer::checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key)
{
	std::lock_guard<std::mutex> lock(*heapMutexRef);	// heap mutex lock
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintMapIterator;
	blueprintMapIterator = blueprintMap.find(in_Key);
	if (blueprintMapIterator != blueprintMap.end())
	{
		//cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	//cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

OSContourPlan* OSServer::getContourPlan(string in_string)
{
	return &contourPlanMap[in_string];		// return a pointer to the plan
}