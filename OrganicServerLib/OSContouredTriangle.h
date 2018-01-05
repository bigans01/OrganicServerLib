#pragma once
#include "OSTrianglePoint.h"

class OSContouredTriangle {
public:
	OSTrianglePoint trianglePoints[3];	// stores 3 points
	OSContouredTriangle(OSTrianglePoint in_point0, OSTrianglePoint in_point1, OSTrianglePoint in_point2);
	OSContouredTriangle();
};
