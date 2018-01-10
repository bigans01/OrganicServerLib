#include "stdafx.h"
#include <iostream>
#include "OSContourPlan.h"


OSContourPlan::OSContourPlan(float in_x, float in_y, float in_z)
{
	startPoint.x = in_x;
	startPoint.y = in_y;
	startPoint.z = in_z;
}

OSContourPlan::OSContourPlan(OSTrianglePoint in_Point)
{
	//startPoint = in_Point;
}

OSContourPlan::OSContourPlan()
{
	// for adding into unordered map
}

OSContourPlan::OSContourPlan(const OSContourPlan& plan_a)
{
	startPoint = plan_a.startPoint;			// must match corresponding = operator
}


void OSContourPlan::addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, OSTrianglePoint in_startPoint)
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
		OSTrianglePoint thirdPointLocation = startPoint;	// set a temporary start point?
		createFirstLayerTriangles();
	}
	
}

void OSContourPlan::createFirstLayerTriangles()
{
	OSContourLine* firstLineRef = &contourLineMap[0];	// get the very first contour line in the plan
	OSTrianglePoint lineCenterPoint = firstLineRef->centerPoint;
	int numberOfPoints = firstLineRef->numberOfPoints;
	int currentStartPoint = 0;

	
	// do the following for all but the last triangle; number of triangles = number of points
	for (int x = 0; x < numberOfPoints - 1; x++)
	{
		//OSContourPoint* contourPointPointer = &PointMap[in_pointIndex];

		// first triangle point
		OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[x];
		OSTrianglePoint pointOne;
		pointOne.x = contourPointPointer->x;
		pointOne.y = contourPointPointer->y;
		pointOne.z = contourPointPointer->z;

		//second triangle point
		contourPointPointer = &firstLineRef->smartContourPoint[x + 1];
		OSTrianglePoint pointTwo;
		pointTwo.x = contourPointPointer->x;
		pointTwo.y = contourPointPointer->y;
		pointTwo.z = contourPointPointer->z;

		//third triangle point
		OSTrianglePoint pointThree;
		pointThree.x = startPoint.x;						// make the center point equal to the "peak" of the contour plan, whatever that may be 
		pointThree.y = startPoint.y;
		pointThree.z = startPoint.z;

		OSContouredTriangle triangleToAdd(pointOne, pointTwo, pointThree);
		triangleStripMap[0].triangleMap[x] = triangleToAdd;

	}
	// do the following for the last triangle only
	int finalPointOne = numberOfPoints - 1;

	OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[finalPointOne];
	OSTrianglePoint pointOne;
	pointOne.x = contourPointPointer->x;
	pointOne.y = contourPointPointer->y;
	pointOne.z = contourPointPointer->z;

	contourPointPointer = &firstLineRef->smartContourPoint[0];
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

	numberOfTriangleStrips++;	// increment number of triangle strips;
	
}

void OSContourPlan::amplifyContourLinePoints(int in_lineID)
{
	contourLineMap[in_lineID].amplifyAllPoints();
}