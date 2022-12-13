#pragma once

#ifndef CONTOUREDTRIANGLEV2_H
#define CONTOUREDTRIANGLEV2_H

#include "EmptyNormalFinder.h"
#include "FTriangle.h"
#include "ForgedPolyRegistry.h"
#include "ECBPolyType.h"

class ContouredTriangleV2
{
	public:
		ContouredTriangleV2();
		ContouredTriangleV2(DoublePoint in_point0,
							DoublePoint in_point1,
							DoublePoint in_point2,
							TriangleMaterial in_triangleMaterial,
							DoublePoint in_massReferencePoint,
							ForgedPolyRegistry* in_allForgedPolysRegistryRef,
							ForgedPolyRegistry* in_shellMassDriverRegistryRef,
							ECBPolyType in_polyType);

		FTriangle produceEquivalentFTriangle();
		ForgedPolyRegistry* allForgedPolysRegistryRef = nullptr;
		ForgedPolyRegistry* shellMassDriverRegistryRef = nullptr;
		ECBPolyType ctv2PolyType = ECBPolyType::FREE;	// default value is FREE, but constructor for triangle should always take in a value to set.
	private:
		DoublePoint points[3];
		TriangleMaterial contouredMaterial = TriangleMaterial::NOVAL;
		DoublePoint ctv2ReferencePoint;

		ECBPolyPoint determineLocalizedEmptyNormal();
		PerfectClampEnum determinePerfectClampValue();
};

#endif
