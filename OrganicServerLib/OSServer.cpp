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

void OSServer::constructTestBlueprints()
{
	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	ECBPolyPoint startPoint;
	startPoint.x = -85.0f;
	startPoint.y = -80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	// Triangle #1 point data
	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;


	// NEW TEST, for fracturing
	testPoint_0.x = -37.4f;
	testPoint_0.y = 6.78f;		// previous: 7.0f		// error with 6.78 here (7/5/2018) --> fixed, 7/9/2018
	testPoint_0.z = 1.00f;		// previous: 0.0f		

	testPoint_1.x = -10.2f;
	testPoint_1.y = 20.80f;			// previous: 18.0f			(7.80, 10.80, 20.80f); 20.80f leads to interesting resutls/angles
	testPoint_1.z = 4.70f;

	testPoint_2.x = -1.23f;
	testPoint_2.y = 7.45f;			// previous: 10.0f
	testPoint_2.z = 27.00f;

	testPoint_3.x = -37.4f;
	testPoint_3.y = 6.78f;
	testPoint_3.z = 17.00f;

	ECBPolyPoint otherTriangle_0, otherTriangle_1, otherTriangle_2;
	otherTriangle_0.x = 16.0f;
	otherTriangle_0.y = 24.0f;
	otherTriangle_0.z = 16.0f;

	otherTriangle_1.x = -16.0f;
	otherTriangle_1.y = 24.0f;
	otherTriangle_1.z = 16.0f;

	otherTriangle_2.x = -16.0f;
	otherTriangle_2.y = 24.0f;
	otherTriangle_2.z = -16.0f;

	// Part 2 of fracturing test:

	ECBPolyPoint otherTriangle2_0, otherTriangle2_1, otherTriangle2_2;
	otherTriangle2_0.x = 16.0f;
	otherTriangle2_0.y = 28.0f;
	otherTriangle2_0.z = 16.0f;

	otherTriangle2_1.x = -16.0f;
	otherTriangle2_1.y = 28.0f;
	otherTriangle2_1.z = 16.0f;

	otherTriangle2_2.x = 16.0f;
	otherTriangle2_2.y = 28.0f;
	otherTriangle2_2.z = 23.0f;

	ECBPolyPoint otherTriangle3_0, otherTriangle3_1, otherTriangle3_2;		// this triangle will be "whole", as in all its points are contained the in the blueprint. 
	otherTriangle3_0.x = -2.0f;
	otherTriangle3_0.y = 30.0f;
	otherTriangle3_0.z = 16.0f;
				 
	otherTriangle3_1.x = -16.0f;
	otherTriangle3_1.y = 30.0f;
	otherTriangle3_1.z = 16.0f;
				 
	otherTriangle3_2.x = -2.0f;
	otherTriangle3_2.y = 30.0f;
	otherTriangle3_2.z = 23.0f;

	ECBPolyPoint mrPoint;
	planRef->constructSingleContouredTriangle(otherTriangle3_0, otherTriangle3_1, otherTriangle3_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(otherTriangle_0, otherTriangle_1, otherTriangle_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(otherTriangle2_0, otherTriangle2_1, otherTriangle2_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::constructTestBlueprints2() 
{
	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
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
	ECBPolyPoint testPoint_4;
	ECBPolyPoint testPoint_5;
	testPoint_0.x = 16.5f;
	testPoint_0.y = 7.3f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
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

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_3, testPoint_4, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_4, testPoint_5, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_5, testPoint_1, mrPoint, 0, 2);
	executeDerivedContourPlan("plan");
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
	
	/*
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
	*/
	
	
	
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

void OSServer::constructTestBlueprints3()
{
	std::cout << "||||||| constructing blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;		
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 0;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 992;
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
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}

	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}


	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
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
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}

	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructTestBlueprints3Special()
{
	std::cout << "||||||| constructing blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 0;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 992;
	mountainSummit.z = 0;


	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)

	int numberOfLayers = 10;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}

	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}


	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructTestBlueprints3Micro()
{
	std::cout << "||||||| constructing blueprints Micro (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	mountainSummit.y = 0.32;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 992.45;		// started at 992.19f.
	mountainSummit.z = 0.67;

	int numberOfLayers = 35;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}

	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}

	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructTestDisk()
{
	ECBPolyPoint summit1;
	int numberOfLayers = 35;

	// first mountain
	summit1.x = 48;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 0, 9, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit1Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}

	executeDerivedContourPlan("summit1");
}

void OSServer::constructMiniTestDisk()
{
	ECBPolyPoint summit1;
	int numberOfLayers = 3;

	// first mountain
	summit1.x = 32;
	summit1.y = 32;
	summit1.z = 32;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 0, 9, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit1Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}

	executeDerivedContourPlan("summit1");
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

	
	
	testPoint_0.x = 32.0f;
	testPoint_0.y = 32.0f;
	testPoint_0.z = 32.0f;

	testPoint_1.x = 41.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 32.0f;

	testPoint_2.x = 32.00f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 41.00f;
	

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
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit1Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}
	for (int x = 0; x < numberOfLayers; x++)
	{
		std::cout << "!!! Calling bottom strips... " << std::endl;
		summit1Ref->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}

	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	executeDerivedContourPlan("summit1");
}

void OSServer::constructSingleMountTestNoInput()
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
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit1Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}
	for (int x = 0; x < numberOfLayers; x++)
	{
		std::cout << "!!! Calling bottom strips... " << std::endl;
		summit1Ref->constructBottomStripTriangles(x, 2);	// construct an individual layer
	}

	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	executeDerivedContourPlanNoInput("summit1");
}

void OSServer::jobSendUpdateMessageToJobManager(Message in_message)
{
	serverJobManager.updateMessages.insertUpdate(in_message);
}

void OSServer::constructMultiMountTest()
{
	ECBPolyPoint summit1, summit2;
	int numberOfLayers = 3;

	// first mountain
	summit1.x = 48;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit1Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}
	executeDerivedContourPlan("summit1");


	// second mountain
	summit2.x = -16;
	summit2.y = 16;
	summit2.z = 16;
	addDerivedContourPlan("summit2", OSTerrainFormation::MOUNTAIN, summit2, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit2Ref = getDerivedContourPlan("summit2");
	summit2Ref->amplifyAllContourLinePoints();
	for (int x = 0; x < numberOfLayers; x++)
	{
		summit2Ref->constructStripTriangles(x, 2);	// construct an individual layer
	}
	executeDerivedContourPlan("summit2");
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

void OSServer::constructTestBlueprints5()
{
	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	// Triangle #1 point data
	ECBPolyPoint startPoint;
	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint massReferencePoint;

	testPoint_0.x = 1047.86;
	testPoint_0.y = -26.17;
	testPoint_0.z = 2.57;

	testPoint_1.x = 1056.38;
	testPoint_1.y = -32.58;
	testPoint_1.z = 2.57;

	testPoint_2.x = 1054.9;
	testPoint_2.y = -32.58;
	testPoint_2.z = -11;

	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	planRef->constructSingleContouredTriangle(testPoint_1, testPoint_0, testPoint_2, massReferencePoint, 0, 5);
	executeDerivedContourPlan("plan");

}

void OSServer::constructTestBlueprints4()
{
	std::cout << "||||||| constructing blueprints (version 4)...." << std::endl;
	ECBPolyPoint point_0;
	point_0.x = 0;
	point_0.y = 0;
	point_0.z = 0;

	ECBPolyPoint point_1;
	point_1.x = 0;
	point_1.y = 0;
	point_1.z = 32;

	ECBPolyPoint point_2;
	point_2.x = 32;
	point_2.y = 0;	// old was 16
	point_2.z = 32;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	planRef->constructSingleContouredTriangle(point_0, point_1, point_2, mrPoint, 0, 5);
	executeDerivedContourPlan("plan");
}

void OSServer::constructDebugBlueprint1()
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

	int numberOfLayers = 27;		// current is 17 (max at 35)
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	/*
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	*/
	planRef->constructStripTriangles(24, 2);	// construct an individual layer
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::executeDerivedContourPlan(string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();
	int numberOfTriangleStrips = planPtr->triangleStripMap.size();
	std::cout << "Number of strips to execute is: " << numberOfTriangleStrips << std::endl;
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapIterator = planPtr->triangleStripMap.begin();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapEnd = planPtr->triangleStripMap.end();

	// 1.) construct the shell
	for (stripMapIterator; stripMapIterator != stripMapEnd; stripMapIterator++)
	{
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = stripMapIterator->second.triangleMap.end();
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			//cout << "Current triangle ID: " << triangleMapIterator->first << endl;
			std::cout << "(SERVER): Executing plan, " + in_string << ", strip: " << stripMapIterator->first << ", triangle: " << triangleMapIterator->first << std::endl;

			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			// DEBUG ONLY, temporary
			//if (triangleMapIterator->first == 79)
			//{
				traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections, &planPtr->adherenceData);
				//std::cout << "###### Debug triangle points: " << std::endl;
				//std::cout << "0: " << currentTriangle->trianglePoints[0].x << ", " << currentTriangle->trianglePoints[0].y << ", " << currentTriangle->trianglePoints[0].z << std::endl;
				//std::cout << "1: " << currentTriangle->trianglePoints[1].x << ", " << currentTriangle->trianglePoints[1].y << ", " << currentTriangle->trianglePoints[1].z << std::endl;
				//std::cout << "2: " << currentTriangle->trianglePoints[2].x << ", " << currentTriangle->trianglePoints[2].y << ", " << currentTriangle->trianglePoints[2].z << std::endl;

				//int someVal = 3;
				//std::cin >> someVal;
			//}
			//std::cout << "---ending trace-through" << std::endl;

			// write (overwrite) a blueprint file for each blueprint traversed
			/*
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorBegin = currentTriangle->polygonPieceMap.begin();
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorEnd = currentTriangle->polygonPieceMap.end();
			for (keyIteratorBegin; keyIteratorBegin != keyIteratorEnd; keyIteratorBegin++)
			{
				EnclaveKeyDef::EnclaveKey currentFileName = keyIteratorBegin->first;	// get the blueprint traversed
				//std::cout << ">> Blueprint file to write is: " << currentFileName.x << ", " << currentFileName.y << ", " << currentFileName.z << ", " << std::endl;
				EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[currentFileName];
				std::map<int, ECBPoly>* polyMapRef = &blueprintRef->primaryPolygonMap;
				//std::map<int, ECBPoly> sillyPolys;
				//polyMapRef = &sillyPolys;
				OSWinAdapter::writeBlueprintPolysToFile(currentWorld, currentFileName, polyMapRef);
				EnclaveCollectionBlueprint readBackBP;
				OSWinAdapter::readBlueprintPolysFromFile(currentWorld, currentFileName, polyMapRef);
				OSWinAdapter::outputBlueprintStats(polyMapRef);
				//std::cout << ">> Blueprint stats outputted...???" << std::endl;
			}
			*/
		}
	}

	auto bottomStripsBegin = planPtr->bottomTriangleStripMap.begin();
	auto bottomStripsEnd = planPtr->bottomTriangleStripMap.end();
	for (bottomStripsBegin; bottomStripsBegin != bottomStripsEnd; bottomStripsBegin++)
	{
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = bottomStripsBegin->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = bottomStripsBegin->second.triangleMap.end();
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections, &planPtr->adherenceData);
		}
	}

	std::cout << "######### Plan execution complete; " << std::endl;

	//planPtr->ad
	//planPtr->adherenceData.printAdherentData();		// testing only, for now.

	//OSWinAdapter::clearWorldFolder(currentWorld);
	
	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &blueprintMap);

	//std::cout << "Size of EnclaveBlock: " << sizeof(EnclaveBlock) << std::endl;
	//std::cout << "Size of BBFan: " << sizeof(BBFan) << std::endl;
	//std::cout << "Size of OrganicRawEnclave: " << sizeof(OrganicRawEnclave) << std::endl;
	//std::cout << "Size of OrganicTriangleSecondary: " << sizeof(OrganicTriangleSecondary) << std::endl;
	//std::cout << "Size of OrganicTriangleTertiary: " << sizeof(OrganicTriangleTertiary) << std::endl;
	//std::cout << "Size of EnclaveTriangle: " << sizeof(EnclaveTriangle) << std::endl;
	//std::cout << "Size of an EnclaveTriangle skeleton: " << sizeof(ECBPolyPoint) * 4 + 4 << std::endl;
	//std::cout << "Size of an ECBPolyLine array of size 3: " << sizeof(ECBPolyLine) * 3 << std::endl;

	//int testVal = 3;
	//std::cin >> testVal;

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &blueprintMap, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&blueprintMap, currentWorld);

	std::cout << "Write to disk complete; press key to transfer blueprints to local OrganicSystem. " << std::endl;
	int someVal = 3;
	std::cin >> someVal;
}

void OSServer::executeDerivedContourPlanNoInput(string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();
	int numberOfTriangleStrips = planPtr->triangleStripMap.size();
	std::cout << "Number of strips to execute is: " << numberOfTriangleStrips << std::endl;
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapIterator = planPtr->triangleStripMap.begin();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapEnd = planPtr->triangleStripMap.end();

	// 1.) construct the shell
	for (stripMapIterator; stripMapIterator != stripMapEnd; stripMapIterator++)
	{
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = stripMapIterator->second.triangleMap.end();
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			//cout << "Current triangle ID: " << triangleMapIterator->first << endl;
			std::cout << "(SERVER): Executing plan, " + in_string << ", strip: " << stripMapIterator->first << ", triangle: " << triangleMapIterator->first << std::endl;

			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			// DEBUG ONLY, temporary
			//if (triangleMapIterator->first == 79)
			//{
			traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections, &planPtr->adherenceData);
			//std::cout << "###### Debug triangle points: " << std::endl;
			//std::cout << "0: " << currentTriangle->trianglePoints[0].x << ", " << currentTriangle->trianglePoints[0].y << ", " << currentTriangle->trianglePoints[0].z << std::endl;
			//std::cout << "1: " << currentTriangle->trianglePoints[1].x << ", " << currentTriangle->trianglePoints[1].y << ", " << currentTriangle->trianglePoints[1].z << std::endl;
			//std::cout << "2: " << currentTriangle->trianglePoints[2].x << ", " << currentTriangle->trianglePoints[2].y << ", " << currentTriangle->trianglePoints[2].z << std::endl;

			//int someVal = 3;
			//std::cin >> someVal;
		//}
		//std::cout << "---ending trace-through" << std::endl;

		// write (overwrite) a blueprint file for each blueprint traversed
		/*
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorBegin = currentTriangle->polygonPieceMap.begin();
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator keyIteratorEnd = currentTriangle->polygonPieceMap.end();
		for (keyIteratorBegin; keyIteratorBegin != keyIteratorEnd; keyIteratorBegin++)
		{
			EnclaveKeyDef::EnclaveKey currentFileName = keyIteratorBegin->first;	// get the blueprint traversed
			//std::cout << ">> Blueprint file to write is: " << currentFileName.x << ", " << currentFileName.y << ", " << currentFileName.z << ", " << std::endl;
			EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[currentFileName];
			std::map<int, ECBPoly>* polyMapRef = &blueprintRef->primaryPolygonMap;
			//std::map<int, ECBPoly> sillyPolys;
			//polyMapRef = &sillyPolys;
			OSWinAdapter::writeBlueprintPolysToFile(currentWorld, currentFileName, polyMapRef);
			EnclaveCollectionBlueprint readBackBP;
			OSWinAdapter::readBlueprintPolysFromFile(currentWorld, currentFileName, polyMapRef);
			OSWinAdapter::outputBlueprintStats(polyMapRef);
			//std::cout << ">> Blueprint stats outputted...???" << std::endl;
		}
		*/
		}
	}

	auto bottomStripsBegin = planPtr->bottomTriangleStripMap.begin();
	auto bottomStripsEnd = planPtr->bottomTriangleStripMap.end();
	for (bottomStripsBegin; bottomStripsBegin != bottomStripsEnd; bottomStripsBegin++)
	{
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = bottomStripsBegin->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = bottomStripsBegin->second.triangleMap.end();
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			OSContouredTriangle* currentTriangle = &triangleMapIterator->second;
			traceTriangleThroughBlueprints(currentTriangle, planPtr->planDirections, &planPtr->adherenceData);
		}
	}

	std::cout << "######### Plan execution complete; " << std::endl;

	//planPtr->ad
	//planPtr->adherenceData.printAdherentData();		// testing only, for now.

	//OSWinAdapter::clearWorldFolder(currentWorld);

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &blueprintMap);

	//std::cout << "Size of EnclaveBlock: " << sizeof(EnclaveBlock) << std::endl;
	//std::cout << "Size of BBFan: " << sizeof(BBFan) << std::endl;
	//std::cout << "Size of OrganicRawEnclave: " << sizeof(OrganicRawEnclave) << std::endl;
	//std::cout << "Size of OrganicTriangleSecondary: " << sizeof(OrganicTriangleSecondary) << std::endl;
	//std::cout << "Size of OrganicTriangleTertiary: " << sizeof(OrganicTriangleTertiary) << std::endl;
	//std::cout << "Size of EnclaveTriangle: " << sizeof(EnclaveTriangle) << std::endl;
	//std::cout << "Size of an EnclaveTriangle skeleton: " << sizeof(ECBPolyPoint) * 4 + 4 << std::endl;
	//std::cout << "Size of an ECBPolyLine array of size 3: " << sizeof(ECBPolyLine) * 3 << std::endl;

	//int testVal = 3;
	//std::cin >> testVal;

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &blueprintMap, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&blueprintMap, currentWorld);

	std::cout << "SERVER: completed contour plan run." << std::endl;
}

void OSServer::transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	organicSystemPtr->addBlueprint(in_key, &blueprintMap[in_key], 0);		// assign constructed blueprint to organic system
}

void OSServer::runPolyFracturer(EnclaveKeyDef::EnclaveKey in_key, PolyDebugLevel in_debugLevel)
{
	auto bpBegin = blueprintMap.begin();
	auto bpEnd = blueprintMap.end();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
	}

	client.fracturePolysInBlueprint(in_key, &blueprintMap[in_key], PolyFractureMode::INITIAL_FILL, in_debugLevel);		// we can set any debug level we want to here.
}

void OSServer::runPolyFracturerForAllBlueprints()
{
	auto bpBegin = blueprintMap.begin();
	auto bpEnd = blueprintMap.end();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
		client.fracturePolysInBlueprint(bpBegin->first, &bpBegin->second, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
	}
}

void OSServer::constructBlueprintFromFile(std::string in_worldName, EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[in_blueprintKey]; // fetch a pointer to the blueprint
	BlueprintTransformRefs  transformRefs(&blueprintRef->primaryPolygonMap,
										  &blueprintRef->fractureResults.fractureResultsContainerMap,
										  &blueprintRef->polyGroupRangeMap);
	OSWinAdapter::readBlueprintPolysFromFile(in_worldName, in_blueprintKey, transformRefs);
}

void OSServer::constructTestBlueprintsForContourBaseTesting()
{
	std::cout << "||||||| constructing blueprints for FRACTURING test.... (NEW ContourBase testing)" << std::endl;
	// std::cout << "LOL WHAT" << std::endl;
	ECBPolyPoint mountainSummit;
	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 1002.45;
	mountainSummit.z = 2.61;
	int numberOfLayers = 2;		// current is 17 (max at 22) // Fatal error at layer 14 when going 1000+x
	//addDerivedContourPlan("mountain1", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	//addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, mountainSummit, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	executeDerivedContourPlan("mountain");
}

void OSServer::constructTestBlueprintsForFracturing()
{
	std::cout << "||||||| constructing blueprints for FRACTURING test...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 1002.45;
	mountainSummit.z = 2.61;

	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)

	int numberOfLayers = 7;		// current is 17 (max at 22) // Fatal error at layer 14 when going 1000+x
	//addContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	//OSContourPlan* planRef = getContourPlan("mountain");		// get pointer to the plan
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");


	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines

	
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructTestBlueprintsForFracturing2()
{
	std::cout << "||||||| constructing blueprints for FRACTURING test...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 1002.45;
	mountainSummit.z = 2.61;

	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)

	int numberOfLayers = 7;		// current is 17 (max at 22) // Fatal error at layer 14 when going 1000+x
	//addContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	//OSContourPlan* planRef = getContourPlan("mountain");		// get pointer to the plan
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");


	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines


	
	for (int x = 0; x < numberOfLayers; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	

	//planRef->constructStripTriangles(5, 2);	// construct an individual layer
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
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


	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintBegin = blueprintMap.begin();
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintEnd = blueprintMap.end();
	for (blueprintBegin; blueprintBegin != blueprintEnd; blueprintBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentKey = blueprintBegin->first;

		//std::cout << "----> Sending blueprint to OrganicSystem: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;

		//std::cout << "Poly groupRange size: " << blueprintBegin->second.polyGroupRangeMap.size() << std::endl;
		//std::cout << "Poly map size: " << blueprintBegin->second.primaryPolygonMap.size() << std::endl;
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

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions, PointAdherenceOrder* in_orderRef)
{
	//calibrateAndRunContouredTriangle(in_Triangle, in_Directions);		// perform calibrations on this single contoured triangle, so that points of the triangle are in the appropriate EnclaveKey
	//determineTriangleType2and3Lines(in_Triangle);		// T-4 cycle through triangle border polys
	OSContouredTriangleRunner runner(in_Triangle, in_Directions, &blueprintMap, in_Triangle->forgedPolyRegistryRef, in_orderRef);
	runner.performRun();
}

void OSServer::writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys)
{
	EnclaveCollectionBlueprint* blueprintRef = &blueprintMap[in_keys];	// get a pointer to the blueprint
	std::map<int, ECBPoly>::iterator currentPrimaryPoly = blueprintRef->primaryPolygonMap.begin();	// beginning of the poly map
	std::map<int, ECBPoly>::iterator primaryPolyEnd = blueprintRef->primaryPolygonMap.end();		// ending of the poly map
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
			checkClientMessages();					// check for requests sent by client

			serverJobManager.checkForUpdateMessages();		// 2.1: check for messages that would be updates to existing jobs, from the previous tick.
			serverJobManager.removeCompletedPhasedJobs();	// 2.2: with the updates applied, check to see if there are any jobs to remove.
			serverJobManager.checkForMessages();			// 2.3: check the messages that would spawn new jobs.
			serverJobManager.runJobScan();					// 2.4: look for jobs to execute.
			//serverJobManager.removeCompletedPhasedJobs();

			//serverJobManager.checkForMessages();	// have the job manager check for messages to process.
			organicSystemPtr->runOrganicTick();		// run core loop
		}
		organicSystemPtr->glCleanup();
	}
}

void OSServer::executeCommandLine()
{
	//int* commandLineRunningRef = &isCommandLineRunning;
	//int* clShutdownStatus = &isCommandLineShutDown;
	//std::future<int> testFuture2 = OSCManager.stemcellMap[0].threadPtr->submit(&OSServer::runCommandLine, this, std::ref(serverReadWrite), std::ref(commandLineCV), std::ref(isCommandLineRunning), std::ref(clShutdownStatus));

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
	blueprintMapIterator = blueprintMap.find(in_Key);
	if (blueprintMapIterator != blueprintMap.end())
	{
		//cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	//cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

/*
OSContourPlan* OSServer::getContourPlan(string in_string)
{
	return &contourPlanMap[in_string];		// return a pointer to the plan
}
*/

ContourBase* OSServer::getDerivedContourPlan(string in_string)
{
	return newContourMap[in_string].get();	// get a reference to the smart pointer
}

void OSServer::checkClientMessages()
{
	client.transferRequestMessages(&serverMessages);							// retrieve incoming messages from the client (via std::move)
	messageInterpreter.interpretIncomingRequestsFromClient();					// interpret (and optionally, process) the messages
}