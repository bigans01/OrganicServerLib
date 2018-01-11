#pragma once
#include "OSTrianglePoint.h"
#include "ECBPolyPoint.h"

class OSTriangleLine
{
public:
	// some comment
	ECBPolyPoint pointA;
	ECBPolyPoint pointB;
	float angleToYaxis = 0.0f;	// gets the angle in degrees, that this line will need when compared to y axis
	float lineLength = 0.0f;
};