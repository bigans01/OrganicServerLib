#include "stdafx.h"
#include <iostream>
#include "OSContourPlan.h"


OSContourPlan::OSContourPlan(float in_x, float in_y, float in_z)
{
	//startPoint.x = in_x;
	//startPoint.y = in_y;
	//startPoint.z = in_z;
}

OSContourPlan::OSContourPlan(OSTrianglePoint in_Point)
{
	//startPoint = in_Point;
}

OSContourPlan::OSContourPlan()
{
	// for adding into unordered map
}


void OSContourPlan::addContourLine(float in_baseDistance, float in_contourElevation, int in_numberOfPoints)
{
	cout << "trolol 6" << endl;
	OSContourLine tempLine(in_baseDistance, in_contourElevation, in_numberOfPoints);
	contourLineMap[contourLineCount] = tempLine;
	//contourLineMap[contourLineCount].smartContourPoint.reset(new OSContourPoint[in_numberOfPoints]);
	contourLineCount++;
	cout << "super trolol" << endl;
}

void OSContourPlan::buildTriangleStrips(int layerDepth)
{
	if (layerDepth == 0)
	{
		OSTrianglePoint thirdPointLocation = startPoint;	// set a temporary start point?
		createFirstLayerTriangles();
	}
}

void OSContourPlan::createFirstLayerTriangles()
{
	OSContourLine* firstLineRef = &contourLineMap[0];	// get the very first contour line in the plan
	int numberOfPoints = firstLineRef->numberOfPoints;
	int currentStartPoint = 0;

	// do the following for all but the last triangle; number of triangles = number of points
	for (int x = 0; x < numberOfPoints - 1; x++)
	{
		//OSContourPoint* contourPointPointer = &PointMap[in_pointIndex];

		// first triangle point
		OSContourPoint* contourPointPointer = &firstLineRef->PointMap[x];
		OSTrianglePoint pointOne;
		pointOne.x = contourPointPointer->x;
		pointOne.y = contourPointPointer->y;
		pointOne.z = contourPointPointer->z;

		//second triangle point
		contourPointPointer = &firstLineRef->PointMap[x + 1];
		OSTrianglePoint pointTwo;
		pointTwo.x = contourPointPointer->x;
		pointTwo.y = contourPointPointer->y;
		pointTwo.z = contourPointPointer->z;

		//third triangle point
		OSTrianglePoint pointThree;
		pointThree.x = startPoint.x;
		pointThree.y = startPoint.y;
		pointThree.z = startPoint.z;

		OSContouredTriangle triangleToAdd(pointOne, pointTwo, pointThree);
		triangleStripMap[0].triangleMap[x] = triangleToAdd;

	}
	// do the following for the last triangle only
	int finalPointOne = numberOfPoints - 1;

	OSContourPoint* contourPointPointer = &firstLineRef->PointMap[finalPointOne];
	OSTrianglePoint pointOne;
	pointOne.x = contourPointPointer->x;
	pointOne.y = contourPointPointer->y;
	pointOne.z = contourPointPointer->z;

	contourPointPointer = &firstLineRef->PointMap[0];
	OSTrianglePoint pointTwo;
	pointTwo.x = contourPointPointer->x;
	pointTwo.y = contourPointPointer->y;
	pointTwo.z = contourPointPointer->z;

	OSTrianglePoint pointThree;
	pointThree.x = startPoint.x;
	pointThree.y = startPoint.y;
	pointThree.z = startPoint.z;

	OSContouredTriangle triangleToAdd(pointOne, pointTwo, pointThree);
	triangleStripMap[0].triangleMap[finalPointOne] = triangleToAdd;
	std::cout << "Triangle strip creation successful." << endl;
}

void OSContourPlan::amplifyContourLinePoints(int in_lineID)
{
	contourLineMap[in_lineID].amplifyAllPoints();
}