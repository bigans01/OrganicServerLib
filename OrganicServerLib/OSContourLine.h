#pragma once
#include "OSAmpField.h"
#include "OSContourPoint.h"
#include <mutex>
#include <math.h>
#include <unordered_map>

#ifndef OSCONTOURLINE_H
#define OSCONTOURLINE_H

using namespace std;
class OSContourLine
{
public:

	OSContourLine(float in_baseDistance, float in_contourElevation, int in_numberOfPoints);
	OSContourLine();	// for adding into unordered_map

	
	OSContourLine& operator=(OSContourLine&& line_a)
	{
		smartContourPoint = std::move(line_a.smartContourPoint);
		//numberOfPoints = line_a.numberOfPoints;
		return *this;
	}
	

	
	OSContourLine& operator=(const OSContourLine& line_a)
	{
		//smartContourPoint = std::move(line_a.smartContourPoint);
		return *this;
	}
	
	
	unordered_map<int, OSAmpField> OSAmpFieldMap;		// contains all amp fields for this contour line
	unordered_map<int, OSContourPoint> PointMap;	// contains all points for the contour line
	unique_ptr<OSContourPoint[]> smartContourPoint;	
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