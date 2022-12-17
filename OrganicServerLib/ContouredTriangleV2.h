#pragma once

#ifndef CONTOUREDTRIANGLEV2_H
#define CONTOUREDTRIANGLEV2_H

#include "EmptyNormalFinder.h"
#include "FTriangle.h"
#include "ForgedPolyRegistry.h"
#include "ECBPolyType.h"

/*

Description:

This class is designed to use a FTriangle to track and output ECBPolys
that would need to be written out to blueprints. It is being designed as an eventual replacement to 
the ContouredTriangle class, and should be much easier to read/understand.

*/

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
		ForgedPolyRegistry* allForgedPolysRegistryRef = nullptr;	// this reference is used to store the keyed sets of all produced ECBPolys.
		ForgedPolyRegistry* shellMassDriverRegistryRef = nullptr;	// this reference is used to store the keyed sets on any ECBPoly that will have a
																	// ECBPolyType of SHELL_MASSDRIVER.
		ECBPolyType ctv2PolyType = ECBPolyType::FREE;	// default value is FREE, but constructor for triangle should always take in a value to set.
	private:
		DoublePoint points[3];
		TriangleMaterial contouredMaterial = TriangleMaterial::NOVAL;
		DoublePoint ctv2ReferencePoint;		// a reference point reflecting the side of the ContouredTriangleV2's plane in which mass will reside.

		ECBPolyPoint determineLocalizedEmptyNormal();	// uses a localized version of the ctv2ReferencePoint 
														// to determine the empty normal value that will be needed by an FTriangle.
		PerfectClampEnum determinePerfectClampValue();	// analyzes the points of this instance of ContouredTriangleV2 to determine if its perfectly clamped.
};

#endif
