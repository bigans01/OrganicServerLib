#include "stdafx.h"
#include <iostream>
#include "OSContouredTriangle.h"

OSContouredTriangle::OSContouredTriangle(OSTrianglePoint in_point0, OSTrianglePoint in_point1, OSTrianglePoint in_point2)
{
	point_0 = in_point0;
	point_1 = in_point1;
	point_2 = in_point2;
}

OSContouredTriangle::OSContouredTriangle()
{
	// for adding to unordered_map
}