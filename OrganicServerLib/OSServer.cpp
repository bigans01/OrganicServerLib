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
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initialize(1, serverSlaves);			// signal for server mode, 2 threads
	serverJobManager.initialize(this);
	OSdirector.initialize(this);
	messageInterpreter.initialize(this, &serverMessages);
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
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initialize(1, serverProperties.serverSlaves);			// signal for server mode, 2 threads
	serverJobManager.initialize(this);
	OSdirector.initialize(this);
	messageInterpreter.initialize(this, &serverMessages);
}

OSServer::~OSServer()
{
	std::cout << "!!! OSServer destructor call BEGIN...." << std::endl;
	OSCManager.despawnAllCells();
	std::cout << "!!! OSServer destructor call END...." << std::endl;
}

void OSServer::addDerivedContourPlan(string in_planName, OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	std::cout << "Adding new, DERIVED contour plan. " << std::endl;
	if (in_Formation == OSTerrainFormation::MOUNTAIN)	// add a mountain
	{
		newContourMap[in_planName].reset(new ContouredMountain());		// for new testing only (11/15/2019)
		newContourMap[in_planName].get()->initialize(in_startPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);		// call the initialization function (common to all ContourBase derivatives)
	}
	if (in_Formation == OSTerrainFormation::NOVAL)		// add a blank plan (must add own triangles)
	{
		newContourMap[in_planName].reset(new BlankContour());
	}
}

void OSServer::constructSingleOrganicTest()
{
	std::cout << "||||||| constructing single poly for OrganicTriangle testing (version 1)...." << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 48.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 48.2f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 48.2f;

	testPoint_2.x = 63.5f;
	testPoint_2.y = 15.5f;
	testPoint_2.z = 48.2f;
	
	testPoint_3.x = 48.0f;
	testPoint_3.y = 15.5f;
	testPoint_3.z = 48.2f;
	*/
	
	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)

	
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 32.0f;

	testPoint_2.x = 32.0f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 32.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 32.0f;
	testPoint_3.z = 0.0f;

	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 0.0f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.0f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 64.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 64.0f;
	testPoint_3.z = 0.0f;
	*/
	
	

	
	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)
	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 32.0f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 32.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 32.0f;
	testPoint_3.z = 32.0f;
	
	
	// |||||||||||||||| Needs testing refinement. (7/14/2020)
	
	testPoint_0.x = 32.0f;
	testPoint_0.y = 32.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 32.0f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.0f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 64.0f;

	testPoint_3.x = 32.0f;
	testPoint_3.y = 64.0f;
	testPoint_3.z = 32.0f;
	*/


	// needs fixing/work; related to the border values used when tracing.
	/*
	testPoint_0.x = 64.0f;
	testPoint_0.y = 64.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 64.0f;

	testPoint_1.x = 96.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 96.0f;

	testPoint_2.x = 96.0f;
	testPoint_2.y = 94.0f;
	testPoint_2.z = 96.0f;
	*/
	
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");

}

void OSServer::constructOrganicRawTest()
{
	std::cout << "!!! Running RAW test! " << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 16.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 16.0f;

	testPoint_1.x = 20.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 16.0f;

	testPoint_2.x = 20.0f;
	testPoint_2.y = 4.0f;
	testPoint_2.z = 16.0f;
	*/

	// a journey into the bizarre below (7/18/2020)
	
	
	testPoint_0.x = 54.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.0f;

	testPoint_1.x = 50.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.0f;

	testPoint_2.x = 50.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.0f;

	testPoint_3.x = 54.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.0f;
	
	
	
	/*
	testPoint_0.x = 49.0f;
	testPoint_0.y = 13.0f;
	testPoint_0.z = 49.0f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 13.0f;
	testPoint_1.z = 49.0f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.0f;

	testPoint_3.x = 50.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.0f;
	*/

	/*
	testPoint_0.x = 50.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.2f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.2f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.2f;

	testPoint_3.x = 50.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.2f;
	*/
	

	/*
	testPoint_0.x = 52.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.2f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.2f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.2f;

	testPoint_3.x = 54.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.2f;
	*/

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::constructBlueprintFillTest()
{

	std::cout << "||||||| Constructing blueprint fill test (version 1)...." << std::endl;
	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = 0.0f;
	startPoint.y = 20.0f;
	startPoint.z = 0.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;

	testPoint_0.x = 16.0f;
	testPoint_0.y = -8.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 4.5f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = -8.0f;
	testPoint_1.z = -16.0f;

	testPoint_2.x = -4.0f;
	testPoint_2.y = 16.0f;
	testPoint_2.z = 4.5f;

	//testPoint_0.x = 16.0f;
	//testPoint_0.y = 16.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	//testPoint_0.z = 4.5f;

	//testPoint_1.x = 0.0f;
	//testPoint_1.y = 16.0f;
	//testPoint_1.z = 4.5f;

	//testPoint_2.x = 8.0f;
	//testPoint_2.y = 12.0f;
	//testPoint_2.z = -4.5f;

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}


void OSServer::constructSingleDebug()
{
	std::cout << "!!! SINGLE debug test. " << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 0.00f;
	testPoint_2.y = 0.0f;
	testPoint_2.z = 32.00f;
	*/

	
	/*
	testPoint_0.x = 32.0f;
	testPoint_0.y = 32.0f;
	testPoint_0.z = 32.0f;

	testPoint_1.x = 41.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 32.0f;

	testPoint_2.x = 32.00f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 41.00f;
	*/

	// for strange behavior detected  on 3/11/2021
	testPoint_0.x = 1;
	testPoint_0.y = 3.57;
	testPoint_0.z = 4;

	testPoint_1.x = 1.75;
	testPoint_1.y = 3.57;
	testPoint_1.z = 1.19;

	testPoint_2.x = 0.8f;
	testPoint_2.y = 3.57;
	testPoint_2.z = 0;
	

	// for reddit question
	/*
	testPoint_0.x = 2.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 0.0f;
	testPoint_1.y = 2.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 0.0f;
	testPoint_2.y = 0.0f;
	testPoint_2.z = 2.0f;
	*/

	// caused bug on 8/20/2020, at ~32000.12, 32000.3, 32000.73.
	// fails when edgeThreshold = .0001 (8/21/2020)

	/*
	testPoint_0.x = 0;
	testPoint_0.y = 1.61;
	testPoint_0.z = 3.06;

	testPoint_1.x = 4;
	testPoint_1.y = 1.54;
	testPoint_1.z = 4;

	testPoint_2.x = 4;
	testPoint_2.y = 0.8;
	testPoint_2.z = 2.98;
	*/

	// fails when edgeThreshold = .00015 (8/21/2020)
	
	/*
	testPoint_0.x = 3.42;
	testPoint_0.y = 0.59;
	testPoint_0.z = 0;

	testPoint_1.x = 0.88;
	testPoint_1.y = 4;
	testPoint_1.z = 4;

	testPoint_2.x = 0;
	testPoint_2.y = 4;
	testPoint_2.z = 2.83;
	*/

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 3.79f;
	testPoint_0.z = 4;


	testPoint_1.x = 0;
	testPoint_1.y = 2.21;
	testPoint_1.z = 0;

	testPoint_2.x = 4;
	testPoint_2.y = 1.15;
	testPoint_2.z = 4;
	*/

	/*
	testPoint_0.x = 64.0f;
	testPoint_0.y = 64.0f;
	testPoint_0.z = 64.0f;

	testPoint_1.x = 73.0f;
	testPoint_1.y = 64.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.00f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 73.00f;
	*/

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::constructMountainAtPoint(float in_summitX, float in_summitY, float in_summitZ, int in_numberOfLayers)
{
	std::cout << "||||||| Constructing Mountain at point, with " << in_numberOfLayers << " layers. " << std::endl;
	ECBPolyPoint mountainSummit;
	mountainSummit.x = in_summitX;
	mountainSummit.y = in_summitY;
	mountainSummit.z = in_summitZ;
	int numberOfLayers = in_numberOfLayers;
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	planRef->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT); // set materials for mountain
	planRef->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructSingleMountTest()
{
	ECBPolyPoint summit1;
	int numberOfLayers = 3;

	// first mountain
	summit1.x = 48;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT); // set materials for mountain
	summit1Ref->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	executeDerivedContourPlan("summit1");
}


void OSServer::constructMultiMountTestWithElevator()
{
	ECBPolyPoint summit1, summit2, summit3;
	int numberOfLayers = 3;

	EnclaveKeyDef::EnclaveKey serverBlueprintKey(0, 0, 0);
	EnclaveKeyDef::EnclaveKey serverBlueprintOREKey(2, 2, 4);

	AdjacentFinder testFinder(&serverBlueprints);

	
	// first mountain
	summit1.x = 28;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT);
	summit1Ref->buildContouredTriangles();
	executeDerivedContourPlan("summit1");

	auto existingOREFinder = serverBlueprints.getFractureResultsMapRef(serverBlueprintKey)->fractureResultsContainerMap.find(serverBlueprintOREKey);

	// tests for AdjacentFinder
	EnclaveKeyDef::EnclaveKey adjacentBeginBlueprintKey(0, 0, 0);
	std::cout << "!!! Findind adjacents for: "; adjacentBeginBlueprintKey.printKey(); std::cout << std::endl;
	auto neighboringBlueprints = testFinder.findAdjacentBlueprints(adjacentBeginBlueprintKey);
	auto foundNeighborsBegin = neighboringBlueprints.begin();
	auto foundNeighborsEnd = neighboringBlueprints.end();
	for (; foundNeighborsBegin != foundNeighborsEnd; foundNeighborsBegin++)
	{
		std::cout << "!! Found neighbor at: "; foundNeighborsBegin->second.blueprintKey.printKey(); std::cout << std::endl;
	}


	if (existingOREFinder != serverBlueprints.getFractureResultsMapRef(serverBlueprintKey)->fractureResultsContainerMap.end())
	{
		std::cout << "::::::: metadata fetch #1 " << std::endl;
		existingOREFinder->second.printMetadata();
	}
	int metadataWait = 3;
	
	std::cin >> metadataWait;
	
	
	// second mountain
	
	summit2.x = 3.33;	// 3.43 = crash? (4/3/2021) --> fixed on 4/5/2021, improved on 4/7/2021, reviewed on 4/8/2021 for smoothness (i.e., removal of hangnails
					// 3.36 = caused a PARTIAL_BOUND to be constructed as a NON_BOUND, due to s/t threshold incorrectly being < 0.000f when it should be < -0.001f, in
					// FusionCandidateProducer::determineRayRelationShipToTriangle (OrganicGLWinLib).
	summit2.y = 16;
	summit2.z = 16;
	addDerivedContourPlan("summit2", OSTerrainFormation::MOUNTAIN, summit2, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit2Ref = getDerivedContourPlan("summit2");
	summit2Ref->amplifyAllContourLinePoints();
	summit2Ref->buildContouredTriangles();
	executeDerivedContourPlan("summit2");

	// debug block test
	EnclaveKeyDef::EnclaveKey targetBlueprintDebug(0, -1, 0);
	EnclaveKeyDef::EnclaveKey targetOREDebug(4, 6, 4);
	//serverBlueprints.getFractureResultsMapRef(targetBlueprintDebug)->fractureResultsContainerMap[targetOREDebug].simulateBlockProduction();
	//serverBlueprints.getFractureResultsMapRef(targetBlueprintDebug)->fractureResultsContainerMap[targetOREDebug].printEnclaveTriangleContainers(true);



	// for debugging test only.
	//EnclaveKeyDef::EnclaveKey serverBlueprintKey(0, -1, 1);
	//EnclaveKeyDef::EnclaveKey serverBlueprintOREKey(5, 7, 0);
	
	//EnclaveKeyDef::EnclaveKey serverBlueprintKey(0, 0, 0);
	//EnclaveKeyDef::EnclaveKey serverBlueprintOREKey(3, 1, 3);



	auto existingOREFinder2 = serverBlueprints.getFractureResultsMapRef(serverBlueprintKey)->fractureResultsContainerMap.find(serverBlueprintOREKey);

	if (existingOREFinder2 != serverBlueprints.getFractureResultsMapRef(serverBlueprintKey)->fractureResultsContainerMap.end())
	{
		std::cout << "::::::: metadata fetch #2 " << std::endl;
		existingOREFinder2->second.printMetadata();
	}
	int metadataWait2 = 3;
	std::cin >> metadataWait2;


	/*
	if (existingOREFinder != blueprintMap[serverBlueprintKey].fractureResults.fractureResultsContainerMap.end())
	{
		existingOREFinder->second.printTriangleMetadata();
	}
	else
	{
		std::cout << "!!! ORE not found. " << std::endl;
	}

	std::cout << "!!!! Finished print test #1...." << std::endl;
	int printWait1 = 3;
	std::cin >> printWait1;
	*/



	//organicSystemPtr->disableColliderOldAppend(EnclaveKeyDef::EnclaveKey(3, 1, 3));
	//organicSystemPtr->disableColliderNewAppend(EnclaveKeyDef::EnclaveKey(2, 2, 4));
	//organicSystemPtr->disableColliderOldAppend(EnclaveKeyDef::EnclaveKey(2, 2, 4));



	// third mountain -- diagonal to other two
	summit3.x = 14.00f;
	summit3.y = 16;
	summit3.z = 28;
	addDerivedContourPlan("summit3", OSTerrainFormation::MOUNTAIN, summit3, numberOfLayers, 6.81, 9, 9);
	ContourBase* summit3Ref = getDerivedContourPlan("summit3");
	summit3Ref->amplifyAllContourLinePoints();
	summit3Ref->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT);
	summit3Ref->buildContouredTriangles();
	//executeDerivedContourPlan("summit3");

	/*
	auto existingOREFinder3 = blueprintMap[serverBlueprintKey].fractureResults.fractureResultsContainerMap.find(serverBlueprintOREKey);
	if (existingOREFinder3 != blueprintMap[serverBlueprintKey].fractureResults.fractureResultsContainerMap.end())
	{
		std::cout << "::::::: metadata fetch #3 " << std::endl;
		existingOREFinder3->second.printMetadata();
	}
	int metadataWait3 = 3;
	std::cin >> metadataWait3;
	*/

	/*
	if (existingOREFinder != blueprintMap[serverBlueprintKey].fractureResults.fractureResultsContainerMap.end())
	{
		existingOREFinder->second.printTriangleMetadata();
	}
	std::cout << "!!!! Finished print test #2...." << std::endl;
	int printWait2 = 3;
	std::cin >> printWait2;
	*/

	// test, convert an ORE's lod to block.
	EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
	EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
	EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
	auto blockReformTarget = serverBlueprints.getFractureResultsMapRef(blockReformBlueprintKey)->fractureResultsContainerMap.find(blockReformOREKey);
	blockReformTarget->second.morphLodToBlock(&serverReadWrite, blockReformOREKey);
	//blockReformTarget->second.eraseBlock(&serverReadWrite, blockKey);
}

void OSServer::constructSingleMountTestNoInput(Message in_metadataMessage)
{
	in_metadataMessage.open();
	std::string planName = in_metadataMessage.readString();
	int mountainLayers = in_metadataMessage.readInt();
	float mountainLocationX = in_metadataMessage.readFloat();
	float mountainLocationY = in_metadataMessage.readFloat();
	float mountainLocationZ = in_metadataMessage.readFloat();
	float distBetweenLayers = in_metadataMessage.readFloat();
	float startRadius = in_metadataMessage.readFloat();
	float expansionValue = in_metadataMessage.readFloat();

	// Part 1: Build the plan, set it up.
	ECBPolyPoint summit1(mountainLocationX, mountainLocationY, mountainLocationZ);
	int numberOfLayers = mountainLayers;
	addDerivedContourPlan(planName, OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, distBetweenLayers, startRadius, expansionValue);	// create the points in all contour lines

	ContourBase* summit1Ref = getDerivedContourPlan(planName);
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT); // set materials for mountain
	summit1Ref->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	runContourPlanWorldTracing(planName);				// Part 2: trace contoured triangles. This should only be called once there are no terrain-modifying jobs running; 
														// As a result, this job must wait for all those to finish, which should be done with 
														// the HALT_FUTURE_COLLECTION_MODIFICATIONS blocking flag being set (this will prevent any future block/modification jobs
														// from running until this flag is no longer set, as long as those jobs check for that flag NOT being set as a pre-requisite to running)

	buildContourPlanAffectedBlueprints(planName);		// Part 3: build affected blueprints.
	runContourPlanFracturingAndMassDriving(planName);	// Part 4: run fracturing and mass driving.
}

void OSServer::constructBigMountTestNoInput()
{
	std::cout << "||||||| constructing blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 0;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 23.59;
	mountainSummit.z = 0;

	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)

	int numberOfLayers = 35;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	planRef->insertMaterials(OSTriangleMaterial::GRASS, OSTriangleMaterial::DIRT);
	planRef->buildContouredTriangles();

	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlanNoInput("mountain");
}



void OSServer::constructMissingFillBlueprint3()
{
	/*
	std::cout << "||||||| constructing 'Missing fill' for blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 1002.45;
	mountainSummit.z = 2.61;

	int numberOfLayers = 35;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	for (int x = 29; x < 30; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
	*/

	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	ECBPolyPoint startPoint;
	startPoint.x = -85.0f;
	startPoint.y = -80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint otherTriangle_0, otherTriangle_1, otherTriangle_2;
	otherTriangle_0.x = 867.89f;
	otherTriangle_0.y = -189.76f;
	otherTriangle_0.z = -221.03f;

	otherTriangle_1.x = 855.4f;
	otherTriangle_1.y = -196.57f;
	otherTriangle_1.z = -223.83f;

	otherTriangle_2.x = 867.45f;
	otherTriangle_2.y = -196.57f;
	otherTriangle_2.z = -231.22f;


	ECBPolyPoint mrPoint;
	planRef->constructSingleContouredTriangle(otherTriangle_0, otherTriangle_1, otherTriangle_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::executeDerivedContourPlan(string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();

	std::cout << "!!!! Size of processable triangles: " << processableList.size() << std::endl;

	auto traceStart = std::chrono::high_resolution_clock::now();

	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	
	std::cout << "######### Plan execution complete; " << std::endl;

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	std::cout << "Write to disk complete; press key to transfer blueprints to local OrganicSystem. " << std::endl;
	int someVal = 3;
	std::cin >> someVal;
}

void OSServer::executeDerivedContourPlanNoInput(std::string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	auto traceStart = std::chrono::high_resolution_clock::now();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();
	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	std::cout << "######### Plan execution complete; " << std::endl;

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	std::cout << "SERVER: completed contour plan run." << std::endl;
}

void OSServer::runContourPlanWorldTracing(std::string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	auto traceStart = std::chrono::high_resolution_clock::now();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();
	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	std::cout << "######### Plan execution complete; " << std::endl;
}

void OSServer::buildContourPlanAffectedBlueprints(std::string in_string)
{

}

void OSServer::runContourPlanFracturingAndMassDriving(std::string in_string)
{
	ContourBase* planPtr = newContourMap[in_string].get();

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	std::cout << "SERVER: completed contour plan run." << std::endl;
}


void OSServer::transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	organicSystemPtr->addBlueprint(in_key, serverBlueprints.getBlueprintRef(in_key), 0);		// assign constructed blueprint to organic system
}

void OSServer::runPolyFracturer(EnclaveKeyDef::EnclaveKey in_key, PolyDebugLevel in_debugLevel)
{
	auto bpBegin = serverBlueprints.getBlueprintBeginIter();
	auto bpEnd = serverBlueprints.getBlueprintEndIter();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
	}

	client.fracturePolysInBlueprint(in_key, serverBlueprints.getBlueprintRef(in_key), PolyFractureMode::INITIAL_FILL, in_debugLevel);		// we can set any debug level we want to here.
}

void OSServer::runPolyFracturerForAllBlueprints()
{
	auto bpBegin = serverBlueprints.getBlueprintBeginIter();
	auto bpEnd = serverBlueprints.getBlueprintEndIter();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
		client.fracturePolysInBlueprint(bpBegin->first, &bpBegin->second, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
	}
}

void OSServer::constructBlueprintFromFile(std::string in_worldName, EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	EnclaveCollectionBlueprint* blueprintRef = serverBlueprints.getBlueprintRef(in_blueprintKey); // fetch a pointer to the blueprint
	BlueprintTransformRefs	transformRefs(blueprintRef->getPolygonMapBeginIter(),
										blueprintRef->getPolygonMapEndIter(),
										blueprintRef->getPolygonMapSize(),
										&blueprintRef->fractureResults.fractureResultsContainerMap,
										&blueprintRef->polyGroupRangeMap);
	BlueprintReadData fetchedData = OSWinAdapter::readBlueprintPolysFromFile(in_worldName, in_blueprintKey, transformRefs);
}

void OSServer::sendAndRenderBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	transferBlueprintToLocalOS(in_key);

	//std::cout  << "----> Sending blueprint to OrganicSystem: " << in_key.x << ", " << in_key.y << ", " << in_key.z << std::endl;
	//organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(in_key);
	organicSystemPtr->addKeyToRenderList(in_key);
}

void OSServer::sendAndRenderAllBlueprintsToLocalOS()
{
	auto organicstart = std::chrono::high_resolution_clock::now();


	auto blueprintBegin = serverBlueprints.getBlueprintBeginIter();
	auto blueprintEnd = serverBlueprints.getBlueprintEndIter();
	for (blueprintBegin; blueprintBegin != blueprintEnd; blueprintBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentKey = blueprintBegin->first;

		//std::cout << "----> Sending blueprint to OrganicSystem: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;

		//std::cout << "Poly groupRange size: " << blueprintBegin->second.polyGroupRangeMap.size() << std::endl;
		//std::cout << "Poly map size: " << blueprintBegin->second.fetchNextECBPolyKeyID() << std::endl;
		//std::cout << "Raw enclave size: " << blueprintBegin->second.fractureResults.fractureResultsContainerMap.size() << std::endl;
		//std::cout << "Skeleton container size: " << blueprintBegin->second.fractureResults.ske.size() << std::endl;

		transferBlueprintToLocalOS(currentKey);
		//organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(currentKey);
		organicSystemPtr->addKeyToRenderList(currentKey);
	}

	auto organicend = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	std::chrono::duration<double> organicelapsed = organicend - organicstart;

	std::cout << "Finished transfering blueprints to local OS. Time: " << organicelapsed.count() << " | Press key to continue. " << std::endl;
	int someVal;
	std::cin >> someVal;
}

void OSServer::jobSendUpdateMessageToJobManager(Message in_message)
{
	serverJobManager.updateMessages.insertUpdate(in_message);
}

void OSServer::jobSendOutgoingMessageToInterpreter(Message in_message)
{
	messageInterpreter.messageCableRef->insertOutgoingMessage(std::move(in_message));
}


void OSServer::setWorldDirectionInClient(float in_directionX, float in_directionY, float in_directionZ)
{
	ECBPolyPoint direction(in_directionX, in_directionY, in_directionZ);
	Message newMessage;
	newMessage.messageType = MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION;
	newMessage.insertPoint(direction);
	// need to insert the client type (localhost, or remotehost) into the message...
	serverJobManager.insertJobRequestMessage(std::move(newMessage));
}

void OSServer::testFunction()
{
	// testingonly
	EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
	EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
	EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
	int currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
	auto currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
	auto currentAppendedState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getAppendedState();
	int currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
	int currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
	std::string state = "";
	std::string appendedState = "";
	if (currentLodState == ORELodState::LOD_BLOCK)
	{
		state = "LOD_BLOCK";
	}

	if (currentAppendedState == OREAppendedState::NONE)
	{
		appendedState = "NONE";
	}

	std::cout << ":: (TEST FUNCTION) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions, PointAdherenceOrder* in_orderRef)
{
	OSContouredTriangleRunner runner(in_Triangle, in_Directions, &serverBlueprints, in_Triangle->forgedPolyRegistryRef, in_orderRef);
	runner.performRun();
}

void OSServer::writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys)
{
	EnclaveCollectionBlueprint* blueprintRef = serverBlueprints.getBlueprintRef(in_keys);	// get a pointer to the blueprint
	std::map<int, ECBPoly>::iterator currentPrimaryPoly = blueprintRef->getPolygonMapBeginIter();	// beginning of the poly map
	std::map<int, ECBPoly>::iterator primaryPolyEnd = blueprintRef->getPolygonMapEndIter();		// ending of the poly map
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
	std::string worldRootFolderString = "world";
	OSWinAdapter::ensureFolderExists(worldRootFolderString);		// make sure the world root folder exists.
	std::string worldFolderString = worldRootFolderString + "/" + in_worldName;
	OSWinAdapter::ensureFolderExists(worldFolderString);			// make sure the folder for this specific world exists.
}

OSPDir OSServer::getFormationDirections(OSTerrainFormation in_terrainFormation)
{
	OSPDir returnDir;
	if (in_terrainFormation == OSTerrainFormation::MOUNTAIN)
	{
		returnDir = OSPDir::BELOW;
		std::cout << "Mountain detected, will OSPDir will be BELOW" << std::endl;
	}
	else
	{
		returnDir = OSPDir::NOVAL;
	}
	return returnDir;
}

void OSServer::runServer()
{
	if (serverRunMode == 0)				// (prototype only, uncertain of use; still needs to be tested as of 2/17/2021) )
	{
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0)	// run until the command line is shutdown
		{
			checkClientMessages();							// the server should still listen for remote client's messages, and handle them accordingly, even though OpenGL isn't active.
		}
	}
	else if (serverRunMode == 1)		// server runs actively alongside an instance of an OrganicSystem
	{
		int currentTickCounter = 0;
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0) // ""
		{
			//std::cout << "Looping...." << std::endl;

			// testingonly
			EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
			EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
			EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
			int currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			auto currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			auto currentAppendedState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getAppendedState();
			int currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			int currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			std::string state = "";
			std::string appendedState = "";
			if (currentLodState == ORELodState::LOD_BLOCK)
			{
				state = "LOD_BLOCK";
			}

			if (currentAppendedState == OREAppendedState::NONE)
			{
				appendedState = "NONE";
			}

			if (currentTickCounter == 0)
			{
				std::cout << "(BEFORE CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
			}

			checkClientMessages();							// Does 3 things: 
															// 1. Move messages coming in from the local client (the underlying OrganicClient on this host), into the messageInterpreter.
															// 2. the messageInterpreter (ServerMessageInterpreter) reads the messsages from local/remote clients, creates the jobs.
															// 3. outgoing messages meant to be sent to the local/remote clients are transferred from the messageInterpreter to their appropriate
															//    MessageCable
													

			serverJobManager.checkForUpdateMessages();		// 2.1: check for messages that would be updates -- or even logical changes to a ServerPhasedJobBase-derived class itself, 
															// such as inserting new ServerJobBase-derived classes  (aka, "atomic" server jobs) into the current job phase -- 
															// to existing jobs, from the previous tick

			serverJobManager.removeCompletedPhasedJobs();	// 2.2: with the updates applied, check to see if there are any jobs to remove.
			serverJobManager.checkForMessages();			// 2.3: check the messages that would spawn new jobs.
			serverJobManager.runJobScan();					// 2.4: look for jobs to execute.
			//serverJobManager.removeCompletedPhasedJobs();

			//serverJobManager.checkForMessages();	// have the job manager check for messages to process.
			//std::cout << ">>>> Running organic tick. " << std::endl;
			organicSystemPtr->runOrganicTick();		// run core loop

			currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			if (currentTickCounter == 0)
			{
				std::cout << "(AFTER CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
				std::cout << ">>>>>>>>>> calling debug test a 2nd time..." << std::endl;
	
			}
			

			// testingonly
			/*
			EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
			EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
			EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
			int currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			auto currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			auto currentAppendedState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getAppendedState();
			int currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			int currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			std::string state = "";
			std::string appendedState = "";
			if (currentLodState == ORELodState::LOD_BLOCK)
			{
				state = "LOD_BLOCK";
			}

			if (currentAppendedState == OREAppendedState::NONE)
			{
				appendedState = "NONE";
			}

			if (currentTickCounter < 50)
			{

				std::cout << "Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
			}
			*/
			currentTickCounter++;
			//std::cout << ">>>> Organic tick done. " << std::endl;
		}
		organicSystemPtr->glCleanup();
	}
}

void OSServer::executeCommandLine()
{
	serverJobManager.startCommandLine();
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

int OSServer::runCommandLineV2(mutex& in_serverReadWrite, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus)
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

void OSServer::runCommandLineV3()
{
	std::cout << ">> Job submission begin..." << std::endl;
	while (isCommandLineRunning == 1)
	{
		std::cout << "Waiting for command...." << std::endl;
		int input_value;
		std::cout << "> ";
		std::cin >> input_value;
		setServerStatus(serverReadWrite, input_value, &isCommandLineRunning);
	}
	signalServerShutdown(serverReadWrite);
	signalCommandLineShutdown(serverReadWrite, 1, &isCommandLineShutDown);
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

int OSServer::checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key)
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintMapIterator;
	blueprintMapIterator = serverBlueprints.getSpecificBlueprintIter(in_Key);
	if (blueprintMapIterator != serverBlueprints.getBlueprintEndIter())
	{
		//cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	//cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

ContourBase* OSServer::getDerivedContourPlan(string in_string)
{
	return newContourMap[in_string].get();	// get a reference to the smart pointer
}

void OSServer::checkClientMessages()
{
	client.transferRequestMessages(&serverMessages);							// retrieve incoming messages from the client (via std::move)
	messageInterpreter.interpretIncomingMessagesFromClient();					// interpret (and optionally, process) the messages from local and/or remote clients
	messageInterpreter.interpretOutgoingMessagesToClient();						// send outgoing messages to local and/or remote clients
}