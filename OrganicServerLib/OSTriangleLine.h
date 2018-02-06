#pragma once

#ifndef OSTRIANGLELINE_H
#define OSTRIANGLELINE_H


#include "OSTrianglePoint.h"
#include "ECBPolyPoint.h"



class OSTriangleLine
{
public:
	// some comment
	ECBPolyPoint pointA;
	ECBPolyPoint pointB;
	ECBPolyPoint x_interceptSlope;
	ECBPolyPoint y_interceptSlope;
	ECBPolyPoint z_interceptSlope;
	char clamped_to_x = 0;			// cache optimized, 8 bytes used only (2 DWORDS)
	char clamped_to_y = 0;
	char clamped_to_z = 0;
	char x_clamp_direction = 0;
	short y_clamp_direction = 0;
	short z_clamp_direction = 0;
	float angleToYaxis = 0.0f;	// gets the angle in degrees, that this line will need when compared to y axis
	float lineLength = 0.0f;
};

#endif