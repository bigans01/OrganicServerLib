#pragma once
#include "OSTrianglePoint.h"

class OSContouredTriangle {
public:
	OSTrianglePoint point_0, point_1, point_2;
	OSContouredTriangle(OSTrianglePoint in_point0, OSTrianglePoint in_point1, OSTrianglePoint in_point2);
	OSContouredTriangle();
};
