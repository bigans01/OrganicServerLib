#pragma once

#ifndef LOCALIZEDPOINTSMETADATA_H
#define LOCALIZEDPOINTSMETADATA_H

#include "EuclideanDirection3D.h"
#include <vector>
#include "ECBPolyPoint.h"

class LocalizedPointsMetaData
{
	public:
		LocalizedPointsMetaData() {};
		LocalizedPointsMetaData(EuclideanDirection3D in_euclideanDirection, std::vector<ECBPolyPoint> in_ecbPolyPointVector) : euclideanDirection(in_euclideanDirection), points(in_ecbPolyPointVector) {};
		EuclideanDirection3D euclideanDirection = EuclideanDirection3D::POS_X;	// it's this by default, but gets reset anyway during constructor initialization list
		std::vector<ECBPolyPoint> points;
};

#endif
