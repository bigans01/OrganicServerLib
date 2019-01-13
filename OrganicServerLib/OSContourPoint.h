#pragma once
#include "ECBPolyPoint.h"

class OSContourPoint
{
public:
	float point_angle = 0.0f;
	float line_length = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	OSContourPoint(float in_angle);
	OSContourPoint(float in_angle, float in_x, float in_y, float in_z);
	OSContourPoint();
	ECBPolyPoint getPolyPoint();
};
