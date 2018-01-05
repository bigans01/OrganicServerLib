#include "stdafx.h"
#include <iostream>
#include <math.h>
#include "OSAmpField.h"


OSAmpField::OSAmpField(float in_phaseShiftDegree, float in_Frequency)
{
	phaseShiftDegree = in_phaseShiftDegree;
	frequency = in_Frequency;
	fullCycleLength = float(2 * PI) / frequency;		// if frequency is 1, this is 6.28~...the full length of the sin wave cycle is 2 pi...
	halfCycleLength = fullCycleLength / 2;		// ...and the half cycle (the "hill") is fullCycleLength / 2.
	endDegree = phaseShiftDegree + piRadsToDegrees(halfCycleLength);	// the end of the hill (at x) will be equivalent to the beginning degree + the length of the half cycle.
	zenithDegree = (endDegree - phaseShiftDegree) / 2;
	std::cout << "Field data, begin degree: " << phaseShiftDegree << " | end degree: " << endDegree << " | zenith degree: " << zenithDegree << std::endl;
}

OSAmpField::OSAmpField()
{
	// for unordered_map (fix later)
}

float OSAmpField::piRadsToDegrees(float in_piRads)
{
	float returnDegrees = in_piRads*RADIAN;
	return returnDegrees;
}

float OSAmpField::getAmplifiedDistance(float in_degree)
{
		float planeAngle = degreesToPiRads(in_degree); // "theta"; convert to pi rads
		float phaseBeginPiRads = degreesToPiRads(phaseShiftDegree);
		float amplifiedValue = sin(frequency*(planeAngle - (phaseBeginPiRads)));	// example: y = sin [ 2(x - pi/4) ], where 2 is the frequency and pi/2 was the phase shift when frequency = 1.
		// std::cout << "amplified value is: " << amplifiedValue << std::endl;
		return amplifiedValue;
}

float OSAmpField::degreesToPiRads(float in_degree)
{
	//float radMultiplier = in_degree / RADIAN;
	float returnRads = in_degree / RADIAN;
	//std::cout << "in_degree is: " << in_degree << std::endl;
	//std::cout << "RADIAN is: " << RADIAN << std::endl;
	//std::cout << "plane Angle will be: " << returnRads << std::endl;
	//std::cout << "test value of 3/4 of a radian: " << 0.75f*RADIAN << std::endl;
	return returnRads;
}
