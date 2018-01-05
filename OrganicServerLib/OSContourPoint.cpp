#include "stdafx.h"
#include <iostream>
#include "OSContourPoint.h"

OSContourPoint::OSContourPoint(float in_angle, float in_x, float in_y, float in_z)
{
	point_angle = in_angle;
	x = in_x;
	y = in_y;
	z = in_z;
}

OSContourPoint::OSContourPoint()
{
	// for inserting into unordered map
}

OSContourPoint::OSContourPoint(float in_angle)
{
	point_angle = in_angle;
}