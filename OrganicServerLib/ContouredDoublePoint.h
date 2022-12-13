#pragma once

#ifndef CONTOUREDDOUBLEPOINT_H
#define CONTOUREDDOUBLEPOINT_H

class ContouredDoublePoint
{
	public:
		float point_angle = 0.0f;
		DoublePoint contouredDPoint;

		ContouredDoublePoint() {};
		ContouredDoublePoint(float in_angle) : point_angle(in_angle) {};
		ContouredDoublePoint(float in_angle, DoublePoint in_contouredDPoint) :	point_angle(in_angle),
																				contouredDPoint(in_contouredDPoint) {};
		DoublePoint getDoublePoint() { return contouredDPoint; }
};

#endif

