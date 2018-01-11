#include "stdafx.h"
#include <iostream>
#include "OSContourLine.h"

OSContourLine::OSContourLine(float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_trianglePoint)
{
	baseDistance = in_baseDistance;
	contourElevation = in_contourElevation;
	numberOfPoints = in_numberOfPoints;
	centerPoint = in_trianglePoint;
	addPoints();
}

OSContourLine::OSContourLine()
{
	// blank constructor for adding into unordered_map
}
/*
OSContourLine::OSContourLine(const OSContourLine& line_a)
{
	OSAmpFieldMap = line_a.OSAmpFieldMap;
	PointMap = line_a.PointMap;
	smartPointIndex = line_a.smartPointIndex;
	baseDistance = line_a.baseDistance;
	contourElevation = line_a.contourElevation;
	OSAmpFieldCurrentIndex = line_a.OSAmpFieldCurrentIndex;
	pointCurrentIndex = line_a.pointCurrentIndex;
	numberOfPoints = line_a.numberOfPoints;
}
*/

void OSContourLine::addPoints()
{
	float degreesBetweenPoints = 360.0f / numberOfPoints;	// get precise degree distance between two points
	float currentPointDegree = 0.0f;						// beginning degree, angle 0
	smartContourPoint.reset(new OSContourPoint[numberOfPoints]);
	for (int x = 0; x < numberOfPoints; x++)
	{
		OSContourPoint pointToAdd(currentPointDegree);	// insert a point equal to the cosine'd x, the contour's elevation, and the sin'd z
		pointToAdd.line_length = baseDistance;			// set initial distance to base distance.
		//cout << "Point added: " << pointToAdd.point_angle << endl;
		smartContourPoint[pointCurrentIndex++] = pointToAdd;
		//PointMap[pointCurrentIndex++] = pointToAdd;
		currentPointDegree += degreesBetweenPoints;
	}

}

void OSContourLine::addOSAmpField(float in_phaseShiftDegree, float in_Frequency)
{
	OSAmpField tempField(in_phaseShiftDegree, in_Frequency);	// create temp field
	OSAmpFieldMap[OSAmpFieldCurrentIndex++] = tempField;		// insert temp field at current index
}

void OSContourLine::amplifyAllPoints()
{
	/*
	unordered_map<int, OSContourPoint>::iterator contourPointIter;
	contourPointIter = PointMap.begin();
	for (contourPointIter; contourPointIter != PointMap.end(); ++contourPointIter)
	{
		int currentIndex = contourPointIter->first;
		float pointAngle = contourPointIter->second.point_angle;
		amplifyPoint(currentIndex, pointAngle);
		//cout << "Amplification result, point " << currentIndex << ": line length: " << contourPointIter->second.line_length << " | angle: " << contourPointIter->second.point_angle << " | x-amp: " << contourPointIter->second.x << " y-elevation: " << contourPointIter->second.y << " z-elevation: " << contourPointIter->second.z << endl;
		//cout << "Amplification result, point " << currentIndex << ": line length: " << contourPointIter->second.line_length << " | angle: " << pointAngle << endl;
	}
	*/
	//cout << "currentPointIndex is: " << pointCurrentIndex << endl;
	for (int x = 0; x < pointCurrentIndex; x++)
	{
		//cout << "attempting to get" << endl;
		float pointAngle = smartContourPoint[x].point_angle;
		amplifyPoint(x, pointAngle);
	}
	cout << "Point amplification complete....." << endl;
}

void OSContourLine::amplifyPoint(int in_pointIndex, float in_pointAngle)
{
	unordered_map<int, OSAmpField>::iterator OSAmpFieldIter;
	OSAmpFieldIter = OSAmpFieldMap.begin();
	int pointWithinField = 0;
	for (OSAmpFieldIter; OSAmpFieldIter != OSAmpFieldMap.end(); ++OSAmpFieldIter)
	{
		if (in_pointAngle >= OSAmpFieldIter->second.phaseShiftDegree			// if the point is greater than/equal to the beginning of the field
			&&
			in_pointAngle <= OSAmpFieldIter->second.endDegree)				// if the point is less than/equal to the end of the field
		{
			float ampedResult = OSAmpFieldIter->second.getAmplifiedDistance(in_pointAngle);
			float finalDistance = baseDistance + (baseDistance*ampedResult);
			float anglePiRads = OSAmpFieldIter->second.degreesToPiRads(in_pointAngle);
			float Xvalue = finalDistance*(cos(anglePiRads));
			float Zvalue = finalDistance*(sin(anglePiRads));

			//cout << "Point is within amp field!" << std::endl;
			//cout << "x is: " << Xvalue << endl;
			//cout << "y is: " << contourElevation << endl;
			//cout << "z is: " << Zvalue << endl;
			//cout << "Final distance line is: " << finalDistance << endl;
			OSContourPoint pointToAdd(in_pointAngle, Xvalue, contourElevation, Zvalue);	// insert a point equal to the cosine'd x, the contour's elevation, and the sin'd z
			//PointMap[pointCurrentIndex] = pointToAdd;
			smartContourPoint[in_pointIndex] = pointToAdd;
			OSContourPoint* contourPointPointer = &smartContourPoint[in_pointIndex];
			contourPointPointer->line_length = finalDistance;
			contourPointPointer->x = Xvalue + centerPoint.x;
			contourPointPointer->y = centerPoint.y;
			contourPointPointer->z = Zvalue + centerPoint.z;
			pointWithinField = 1;
		}
	}
	if (pointWithinField == 0)
	{
		float finalDistance = baseDistance;
		OSAmpField tempField;
		float anglePiRads = tempField.degreesToPiRads(in_pointAngle);
		float Xvalue = finalDistance*(cos(anglePiRads));
		float Zvalue = finalDistance*(sin(anglePiRads));

		OSContourPoint* contourPointPointer = &smartContourPoint[in_pointIndex];
		contourPointPointer->line_length = finalDistance;
		contourPointPointer->x = Xvalue + centerPoint.x;
		contourPointPointer->y = centerPoint.y;
		contourPointPointer->z = Zvalue + centerPoint.z;
		//cout << "Final distance line is: (no amp field) " << finalDistance << endl;
	}
}


