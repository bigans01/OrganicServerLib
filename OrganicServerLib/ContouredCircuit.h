#pragma once

#ifndef ContouredCircuit_H
#define ContouredCircuit_H

#include "ContouredDoublePoint.h"
#include "OSAmpField.h"

class ContouredCircuit
{
	public:
		ContouredCircuit() {};	// required default constructor, since we're overloading the normal operators.
		ContouredCircuit(float in_baseDistance, 
						double in_contourElevation, 
						int in_numberOfPoints, 
						DoublePoint in_trianglePoint);

		ContouredCircuit& operator=(const ContouredCircuit& line_b)
		{
			ampFieldMap = line_b.ampFieldMap;
			smartPointIndex = line_b.smartPointIndex;
			baseDistance = line_b.baseDistance;
			contourElevation = line_b.contourElevation;
			ampFieldCurrentIndex = line_b.ampFieldCurrentIndex;
			pointCurrentIndex = line_b.pointCurrentIndex;				// <---- error is here 
			numberOfPoints = line_b.numberOfPoints;
			circuitCenterPoint = line_b.circuitCenterPoint;
			smartContourPoint.reset(new ContouredDoublePoint[numberOfPoints]);
			for (int x = 0; x < numberOfPoints; x++)
			{
				smartContourPoint[x] = line_b.smartContourPoint[x];
			}
			return *this;
		}

		std::unordered_map<int, OSAmpField> ampFieldMap;		// contains all amp fields for this contour line
		std::unique_ptr<ContouredDoublePoint[]> smartContourPoint;
		DoublePoint circuitCenterPoint;
		int smartPointIndex = 0;					// keeps track of current index for inserting points
		float baseDistance = 0.0f;					// the base distance between the center and outlying points
		double contourElevation;						// the elevation, or "y" value for this contour line
		int ampFieldCurrentIndex = 0;				// a value to determine what the key value for each OSAmpField will be when inserted into OSAmpFieldMap.
		int pointCurrentIndex = 0;
		int numberOfPoints = 0;

		void amplifyAllPoints();
	private:
		void addPoints();
		void addOSAmpField(float in_phaseShiftDegree, float in_frequency);
		void amplifyPoint(int in_pointIndex, float in_pointAngle);

};

#endif
