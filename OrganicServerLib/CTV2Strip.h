#pragma once

#ifndef CTV2STRIP_H
#define CTV2STRIP_H

#include "ContouredTriangleV2.h"

class CTV2Strip
{
	public:
		CTV2Strip() {};
		std::unordered_map<int, ContouredTriangleV2> triangleMap;
};

#endif