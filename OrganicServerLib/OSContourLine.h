#pragma once
#include "OSAmpField.h"
#include "OSContourPoint.h"
#include "OSTrianglePoint.h"
#include "ECBPolyPoint.h"
#include <mutex>
#include <math.h>
#include <unordered_map>

#ifndef OSCONTOURLINE_H
#define OSCONTOURLINE_H

using namespace std;
class OSContourLine
{
public:

	OSContourLine(float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_trianglePoint);
	OSContourLine();	// for adding into unordered_map
	//OSContourLine(const OSContourLine& line_a);

	
	OSContourLine& operator=(OSContourLine&& line_a)
	{
		smartContourPoint = std::move(line_a.smartContourPoint);
		OSAmpFieldMap = line_a.OSAmpFieldMap;
		smartPointIndex = line_a.smartPointIndex;
		baseDistance = line_a.baseDistance;
		contourElevation = line_a.contourElevation;
		OSAmpFieldCurrentIndex = line_a.OSAmpFieldCurrentIndex;
		pointCurrentIndex = line_a.pointCurrentIndex;				
		numberOfPoints = line_a.numberOfPoints;
		centerPoint = line_a.centerPoint;
		return *this;
	}
	

	
	OSContourLine& operator=(const OSContourLine& line_a)
	{
		//smartContourPoint = std::move(line_a.smartContourPoint);
		

		OSAmpFieldMap = line_a.OSAmpFieldMap;
		smartPointIndex = line_a.smartPointIndex;
		baseDistance = line_a.baseDistance;
		contourElevation = line_a.contourElevation;
		OSAmpFieldCurrentIndex = line_a.OSAmpFieldCurrentIndex;
		pointCurrentIndex = line_a.pointCurrentIndex;				// <---- error is here 
		numberOfPoints = line_a.numberOfPoints;
		centerPoint = line_a.centerPoint;
		smartContourPoint.reset(new OSContourPoint[numberOfPoints]);
		for (int x = 0; x < numberOfPoints; x++)
		{
			smartContourPoint[x] = line_a.smartContourPoint[x];
		}
		return *this;
	}
	
	
	unordered_map<int, OSAmpField> OSAmpFieldMap;		// contains all amp fields for this contour line
	unique_ptr<OSContourPoint[]> smartContourPoint;	
	ECBPolyPoint centerPoint;
	int smartPointIndex = 0;					// keeps track of current index for inserting points
	float baseDistance = 0.0f;					// the base distance between the center and outlying points
	float contourElevation;						// the elevation, or "y" value for this contour line
	int OSAmpFieldCurrentIndex = 0;				// a value to determine what the key value for each OSAmpField will be when inserted into OSAmpFieldMap.
	int pointCurrentIndex = 0;
	int numberOfPoints = 0;

	

	void addOSAmpField(float in_phaseShiftDegree, float in_Frequency);
	void amplifyPoint(int in_pointIndex, float in_pointAngle);
	void amplifyAllPoints();
	void addPoints();

};

#endif