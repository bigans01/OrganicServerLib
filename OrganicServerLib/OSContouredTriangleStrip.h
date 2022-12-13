#pragma once
#include "OSContouredTriangle.h"

class OSContouredTriangleStrip
{
public:
	std::unordered_map<int, OSContouredTriangle> triangleMap;
};
