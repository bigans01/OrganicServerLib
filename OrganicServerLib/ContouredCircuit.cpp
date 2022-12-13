#include "stdafx.h"
#include "ContouredCircuit.h"

ContouredCircuit::ContouredCircuit(float in_baseDistance,
								double in_contourElevation,
								int in_numberOfPoints,
								DoublePoint in_trianglePoint)
{
	baseDistance = in_baseDistance;
	contourElevation = in_contourElevation;
	numberOfPoints = in_numberOfPoints;
	circuitCenterPoint = in_trianglePoint;
	circuitCenterPoint.y = in_contourElevation;
	addPoints();
}

void ContouredCircuit::addPoints()
{
	float degreesBetweenPoints = 360.0f / numberOfPoints;	// get precise degree distance between two points
	float currentPointDegree = 0.0f;						// beginning degree, angle 0
	smartContourPoint.reset(new ContouredDoublePoint[numberOfPoints]);
	for (int x = 0; x < numberOfPoints; x++)
	{
		ContouredDoublePoint pointToAdd(currentPointDegree);
		smartContourPoint[pointCurrentIndex++] = pointToAdd;
		currentPointDegree += degreesBetweenPoints;
	}
}

void ContouredCircuit::addOSAmpField(float in_phaseShiftDegree, float in_frequency)
{
	OSAmpField tempField(in_phaseShiftDegree, in_frequency);	// create temp field
	ampFieldMap[ampFieldCurrentIndex++] = tempField;		// insert temp field at current index
}

void ContouredCircuit::amplifyPoint(int in_pointIndex, float in_pointAngle)
{
	bool wasWithinAmpField = false;

	// The below logic for working with ampfields will very likely need to be reworked, as it has not been significantly 
	// tested. (Noted on 12/9/2022)
	for (auto& currentAmpField : ampFieldMap)
	{
		if 
		(
			in_pointAngle >= currentAmpField.second.phaseShiftDegree	// if the point is greater than/equal to the beginning of the field
			&&
			in_pointAngle <= currentAmpField.second.endDegree			// if the point is less than/equal to the end of the field
		)				
		{
			float ampedResult = currentAmpField.second.getAmplifiedDistance(in_pointAngle);
			float finalDistance = baseDistance + (baseDistance*ampedResult);
			float anglePiRads = currentAmpField.second.degreesToPiRads(in_pointAngle);
			float xValue = finalDistance * (cos(anglePiRads));
			float zValue = finalDistance * (sin(anglePiRads));

			DoublePoint point(double(xValue), contourElevation, double(zValue));
			ContouredDoublePoint pointToAdd(in_pointAngle, point);

			smartContourPoint[in_pointIndex] = pointToAdd;
			auto currentPointRef = &smartContourPoint[in_pointIndex];
			currentPointRef->contouredDPoint.x = xValue;
			currentPointRef->contouredDPoint.y = circuitCenterPoint.y;
			currentPointRef->contouredDPoint.z = zValue;
			currentPointRef->contouredDPoint.roundHundredth();

			wasWithinAmpField = true;	// no amplification required outside of this loop (see below)
		}
	}

	// if there were no amp fields, or the point was not found in any existing amp fields, then just use the unmodified base distance.
	if (!wasWithinAmpField)
	{
		float finalDistance = baseDistance;
		OSAmpField tempField;
		float anglePiRads = tempField.degreesToPiRads(in_pointAngle);
		float xValue = finalDistance * (cos(anglePiRads));
		float zValue = finalDistance * (sin(anglePiRads));

		auto currentPointRef = &smartContourPoint[in_pointIndex];
		currentPointRef->contouredDPoint.x = xValue;
		currentPointRef->contouredDPoint.y = circuitCenterPoint.y;
		currentPointRef->contouredDPoint.z = zValue;
		currentPointRef->contouredDPoint.roundHundredth();
	}
}

void ContouredCircuit::amplifyAllPoints()
{
	for (int x = 0; x < pointCurrentIndex; x++)
	{
		//cout << "attempting to get" << endl;
		float pointAngle = smartContourPoint[x].point_angle;
		amplifyPoint(x, pointAngle);
	}
	cout << "(ContouredCircuit) Point amplification complete....." << endl;
}