#pragma once
#include "OSTrianglePoint.h"

class OSTriangleLine
{
public:
	// some comment
	OSTrianglePoint pointA;
	OSTrianglePoint pointB;
	float angleToYaxis = 0.0f;	// gets the angle in degrees, that this line will need when compared to y axis
	float lineLength = 0.0f;
};