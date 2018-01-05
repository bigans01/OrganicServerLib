#pragma once
#include "OSContouredTriangle.h"
#include <unordered_map>

using namespace std;
class OSContouredTriangleStrip
{
public:
	unordered_map<int, OSContouredTriangle> triangleMap;
};
