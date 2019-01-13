#include "stdafx.h"
#include <iostream>
#include "OSContourPlan.h"


OSContourPlan::OSContourPlan(OSPDir in_Dir, float in_x, float in_y, float in_z)
{
	startPoint.x = in_x;
	startPoint.y = in_y;
	startPoint.z = in_z;
	if (in_Dir == OSPDir::BELOW)		// default direction would be "below"
	{
		planDirections.y_direction = -1;
	}
	else if (in_Dir == OSPDir::ABOVE)
	{
		planDirections.y_direction = 1;
	}
	else if (in_Dir == OSPDir::WEST)
	{
		planDirections.x_direction = -1;
	}
	else if (in_Dir == OSPDir::EAST)
	{
		planDirections.x_direction = 1;
	}
	else if (in_Dir == OSPDir::NORTH)
	{
		planDirections.z_direction = -1;
	}
	else if (in_Dir == OSPDir::SOUTH)
	{
		planDirections.z_direction = 1;
	}
}

OSContourPlan::OSContourPlan(OSTrianglePoint in_Point)
{
	//startPoint = in_Point;
}

OSContourPlan::OSContourPlan(ECBPolyPoint in_Point)
{

}

OSContourPlan::OSContourPlan(OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	terrainFormation = in_Formation;
	startPoint = in_startPoint;

	if (in_Formation == OSTerrainFormation::MOUNTAIN)
	{
		// set up the first contour line
		float currentYElevation = in_startPoint.y - in_distanceBetweenLayers;
		addContourLine(contourLineCount, in_startRadius, currentYElevation, 4, in_startPoint);
		amplifyContourLinePoints(0);
		std::cout << "Preparing cycle..." << std::endl;
		for (int x = 0; x < 4; x++)
		{
			std::cout << x << ": " << "(" << contourLineMap[0].smartContourPoint[x].x << ", " << contourLineMap[0].smartContourPoint[x].y << ", " << contourLineMap[0].smartContourPoint[x].z << ") " << std::endl;
		}
	}
}

OSContourPlan::OSContourPlan()
{
	// for adding into unordered map
}

OSContourPlan::OSContourPlan(const OSContourPlan& plan_a)
{
	startPoint = plan_a.startPoint;			// must match corresponding = operator
}

void OSContourPlan::performSingleTriangleTest()
{
	//triangleStripMap[0].triangleMap = c
}

void OSContourPlan::setFormation(OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	terrainFormation = in_Formation;
	startPoint = in_startPoint;

	if (in_Formation == OSTerrainFormation::MOUNTAIN)
	{
		// set up the first contour line
		float currentYElevation = in_startPoint.y - in_distanceBetweenLayers;
		addContourLine(contourLineCount, in_startRadius, currentYElevation, 4, in_startPoint);
		amplifyContourLinePoints(0);
		std::cout << "Preparing cycle..." << std::endl;
		for (int x = 0; x < 4; x++)
		{
			std::cout << x << ": " << "(" << contourLineMap[0].smartContourPoint[x].x << ", " << contourLineMap[0].smartContourPoint[x].y << ", " << contourLineMap[0].smartContourPoint[x].z << ") " << std::endl;
		}
	}
}

void OSContourPlan::constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, int in_triangleStripID, short in_materialID, mutex& heapmutex)
{
	std::lock_guard<std::mutex> lock(heapmutex);	// lock for heap usage
	/*
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
	*/
	OSContouredTriangle testTriangle;
	testTriangle.trianglePoints[0] = in_x;
	testTriangle.trianglePoints[1] = in_y;
	testTriangle.trianglePoints[2] = in_z;
	testTriangle.materialID = in_materialID;		// set the material of the triangle
	testTriangle.determineLineLengths();
	testTriangle.determineAxisInterceptDistances();
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

		//currentTriPoint.triPoints[x] = testTriangle.trianglePoints[x];		// add this point and its assumed precise blueprint key
		//currentTriKey.triKey[x] = blueprintKey;
		testTriangle.pointKeys[x] = blueprintKey;
	}
	int baseStripSize = triangleStripMap[in_triangleStripID].triangleMap.size();		// get the number of triangles in the base strip, should be 0
	std::cout << "### Adding new triangle with ID " << baseStripSize << std::endl;
	triangleStripMap[0].triangleMap[baseStripSize] = testTriangle;
	std::cout << "### New size is: " << triangleStripMap[in_triangleStripID].triangleMap.size() << std::endl;
}

void OSContourPlan::addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint)
{
	//cout << "trolol 6" << endl;
	OSContourLine tempLine(in_baseDistance, in_contourElevation, in_numberOfPoints, in_startPoint);
	contourLineMap[line_id] = tempLine;
	contourLineCount++;
	//cout << "super trolol" << endl;
}

OSContourLine* OSContourPlan::getContourLine(int in_lineId)
{
	//return std::ref(contourLineMap[in_lineId]);
	return  &contourLineMap[in_lineId];
}

void OSContourPlan::buildTriangleStrips(int layerDepth)
{
	if (layerDepth == 0)
	{
		//OSTrianglePoint thirdPointLocation = startPoint;	// set a temporary start point?
		createFirstLayerTriangles();
	}
	
}

void OSContourPlan::createFirstLayerTriangles()
{
	OSContourLine* firstLineRef = &contourLineMap[0];	// get the very first contour line in the plan
	ECBPolyPoint lineCenterPoint = firstLineRef->centerPoint;
	int numberOfPoints = firstLineRef->numberOfPoints;
	int currentStartPoint = 0;

	
	// do the following for all but the last triangle; number of triangles = number of points
	for (int x = 0; x < numberOfPoints - 1; x++)
	{
		//OSContourPoint* contourPointPointer = &PointMap[in_pointIndex];

		// first triangle point
		OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[x];
		ECBPolyPoint pointOne;
		pointOne.x = contourPointPointer->x;
		pointOne.y = contourPointPointer->y;
		pointOne.z = contourPointPointer->z;

		//second triangle point
		contourPointPointer = &firstLineRef->smartContourPoint[x + 1];
		ECBPolyPoint pointTwo;
		pointTwo.x = contourPointPointer->x;
		pointTwo.y = contourPointPointer->y;
		pointTwo.z = contourPointPointer->z;

		//third triangle point
		ECBPolyPoint pointThree;
		pointThree.x = startPoint.x;						// make the center point equal to the "peak" of the contour plan, whatever that may be 
		pointThree.y = startPoint.y;
		pointThree.z = startPoint.z;

		OSContouredTriangle triangleToAdd(pointOne, pointTwo, pointThree);
		triangleStripMap[0].triangleMap[x] = triangleToAdd;

	}
	// do the following for the last triangle only
	int finalPointOne = numberOfPoints - 1;

	OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[finalPointOne];
	ECBPolyPoint pointOne;
	pointOne.x = contourPointPointer->x;
	pointOne.y = contourPointPointer->y;
	pointOne.z = contourPointPointer->z;

	contourPointPointer = &firstLineRef->smartContourPoint[0];
	ECBPolyPoint pointTwo;
	pointTwo.x = contourPointPointer->x;
	pointTwo.y = contourPointPointer->y;
	pointTwo.z = contourPointPointer->z;

	ECBPolyPoint pointThree;
	pointThree.x = startPoint.x;
	pointThree.y = startPoint.y;
	pointThree.z = startPoint.z;

	OSContouredTriangle triangleToAdd(pointOne, pointTwo, pointThree);
	triangleStripMap[0].triangleMap[finalPointOne] = triangleToAdd;
	std::cout << "Triangle strip creation successful." << endl;

	numberOfTriangleStrips++;	// increment number of triangle strips;
	
}

void OSContourPlan::amplifyContourLinePoints(int in_lineID)
{
	contourLineMap[in_lineID].amplifyAllPoints();
}