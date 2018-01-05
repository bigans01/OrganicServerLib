#pragma once
#include <math.h>
#define PI 3.14159265f
#define RADIAN 57.2958f

class OSAmpField
{
public:
	float phaseShiftDegree;		// the value of the phase shift, in degrees for this field
	float endDegree;			// the ending degree for this field
	float frequency = 1.0f;		// the frequency of the entire wave (default would be 1.0f
	float halfPeriod;			// a value equal to half the period of the sin wave
	float halfCycleLength;		// equiavlent to half the cycle (so only the distance for one positive "hill")
	float fullCycleLength;		// equivalent to the length of a full cycle.
	float zenithDegree;			// the point in the field where the amplification is at its highest


	OSAmpField(float in_phaseShiftDegree, float in_Frequency);		// default constructor
	OSAmpField();

	float piRadsToDegrees(float in_piRads);							// takes in pi rads, returns the degree equivalent
	float degreesToPiRads(float in_degree);
	float getAmplifiedDistance(float in_degree);
};