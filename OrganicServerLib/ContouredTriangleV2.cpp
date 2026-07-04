#include "stdafx.h"
#include "ContouredTriangleV2.h"

ContouredTriangleV2::ContouredTriangleV2()
{

}

ContouredTriangleV2::ContouredTriangleV2(DoublePoint in_point0,
										DoublePoint in_point1,
										DoublePoint in_point2,
										TriangleMaterial in_triangleMaterial,
										DoublePoint in_massReferencePoint,
										ForgedPolyRegistry* in_allForgedPolysRegistryRef,
										ForgedPolyRegistry* in_shellMassDriverRegistryRef,
										ECBPolyType in_polyType)
{
	points[0] = in_point0;
	points[1] = in_point1;
	points[2] = in_point2;

	// round to hundredth, so we can do equivalent expressions
	for (int x = 0; x < 3; x++)
	{
		points[x].roundHundredth();
	}

	contouredMaterial = in_triangleMaterial;
	ctv2ReferencePoint = in_massReferencePoint;
	allForgedPolysRegistryRef = in_allForgedPolysRegistryRef;
	shellMassDriverRegistryRef = in_shellMassDriverRegistryRef;
	ctv2PolyType = in_polyType;

	//ECBPolyPoint normalPoint = determineLocalizedEmptyNormal();
	//PerfectClampEnum clampValue = determinePerfectClampValue();

	/*
	if (ctv2PolyType == ECBPolyType::SHELL)
	{
		ECBPolyPoint determinedEmptyNormal = determineLocalizedEmptyNormal();
		if (determinedEmptyNormal.y < 0.0f)
		{
			std::cout << "#####################################" << std::endl;
			std::cout << "!!!! ##### NOTICE -> found ECBPolyType::SHELL with negative y in empty normal, in ContouredTriangleV2::ContouredTriangleV2!!! " << std::endl;
			std::cout << "#####################################" << std::endl;

			std::cout << "Points: " << std::endl;
			for (int x = 0; x < 3; x++)
			{
				std::cout << x << " -> ";
				points[x].printPointCoords();
				std::cout << std::endl;
			}

			std::cout << "Mass reference point: " << std::endl;
			ctv2ReferencePoint.printPointCoords();
			std::cout << std::endl;

			std::cout << "Determined empty normal:" << std::endl;
			determinedEmptyNormal.printPointCoords();
			std::cout << std::endl;



			std::cout << "----------------------Debug empty normal----------------------" << std::endl;
			determineLocalizedEmptyNormalV2Debug();

			std::cout << "Enter number to continue..." << std::endl;
			int polyDebugWait = 3;
			std::cin >> polyDebugWait;
		}
	}
	*/
}

FTriangle ContouredTriangleV2::produceEquivalentFTriangle()
{
	// Remember, a world-produced FTriangle can't have its BoundaryOrientation determined, as that can only be done when
	// we have a guarantee that the triangle will always be within the bounds of a single Blueprint area. And we don't have that 
	// guarantee, since an FTriangle having the type FTriangleType::WORLD can go into multiple blueprints.
	//
	// However, the FTriangleOutput instances are analyzed to determine what their BoundaryOrientation is, if any.

	return FTriangle(points[0],
					points[1],
					points[2],
					FTriangleType::WORLD,
					//determineLocalizedEmptyNormal(),
					determineLocalizedEmptyNormal(),
					BoundaryOrientation::NONE,
					determinePerfectClampValue(),
					contouredMaterial);
}


ECBPolyPoint ContouredTriangleV2::determineLocalizedEmptyNormalV2Debug()
{

	std::set<double> xRanges;
	std::set<double> yRanges;
	std::set<double> zRanges;

	for (int x = 0; x < 3; x++)
	{
		xRanges.insert(points[x].x);
		yRanges.insert(points[x].y);
		zRanges.insert(points[x].z);

		//std::cout << "Double point values were: " << points[x].x << ", " << points[x].y << ", " << points[x].z << std::endl;
	}

	// the amount to adjust in x/y/z, is the inverse of the minimum found in each dim set.
	// I.e., a triangle point at 320022.0f is going to need to be adjusted by -320000.0f
	double minXAdjustment = ((floor(*xRanges.begin() / 32.0)) * -1.0) * 32.0;
	double minYAdjustment = ((floor(*yRanges.begin() / 32.0)) * -1.0) * 32.0;
	double minZAdjustment = ((floor(*zRanges.begin() / 32.0)) * -1.0) * 32.0;


	DoublePoint translationValue(minXAdjustment, minYAdjustment, minZAdjustment);
	translationValue.roundHundredth();

	//std::cout << "translationValues are:" << translationValue.x << ", " << translationValue.y << ", " << translationValue.z << std::endl;

	// now, translate copies of the points (not the original points), along with the MRP.
	DoublePoint adjustedPoints[3];
	for (int x = 0; x < 3; x++)
	{
		adjustedPoints[x] = points[x] + translationValue;
	}
	DoublePoint adjustedMRP = ctv2ReferencePoint + translationValue;

	std::cout << "!! DEBUG: translationValue is: ";
	translationValue.printPointCoords(); 
	std::cout << std::endl;

	
	
	DoublePoint determinedCentroid = IndependentUtils::determineTriangleCentroid(adjustedPoints[0], adjustedPoints[1], adjustedPoints[2]);

	std::cout << "!! DEBUG: centroid, post-translation is: ";
	determinedCentroid.printPointCoords(); 
	std::cout << std::endl;

	std::cout << "!! DEBUG: MRP, post-translation is: ";
	adjustedMRP.printPointCoords();
	std::cout << std::endl;

	std::cout << "!! DEBUG: centorid - MRP is: ";
	DoublePoint diff = determinedCentroid - adjustedMRP;
	diff.printPointCoords();
	std::cout << std::endl;

	glm::vec3 diffToNormalize(diff.x, diff.y, diff.z);
	diffToNormalize = glm::normalize(diffToNormalize);

	std::cout << "!! DEBUG: diffToNormalize final value is: " << std::endl;
	std::cout << diffToNormalize.x << ", " << diffToNormalize.y << ", " << diffToNormalize.z << std::endl;



	return ECBPolyPoint(diffToNormalize.x, diffToNormalize.y, diffToNormalize.z);

}

ECBPolyPoint ContouredTriangleV2::determineLocalizedEmptyNormal()
{
	EmptyNormalFinder finder(points[0], points[1], points[2], ctv2ReferencePoint);
	return ECBPolyPoint(finder.getFinderCalculatedNormal());
}

PerfectClampEnum ContouredTriangleV2::determinePerfectClampValue()
{
	PerfectClampEnum determinedClampValue = PerfectClampEnum::NONE;

	if 
	(
		(points[0].x == points[1].x)
		&&
		(points[0].x == points[2].x)
	)
	{
		determinedClampValue = PerfectClampEnum::CLAMPED_TO_X;
		//std::cout << "(ContouredTriangleV2::determinePerfectClampValue()) -> this instance of ContouredTriangleV2 found as being CLAMPED_TO_X. " << std::endl;
	}

	else if 
	(
		(points[0].y == points[1].y)
		&&
		(points[0].y == points[2].y)
	)
	{
		determinedClampValue = PerfectClampEnum::CLAMPED_TO_Y;
		//std::cout << "(ContouredTriangleV2::determinePerfectClampValue()) -> this instance of ContouredTriangleV2 found as being CLAMPED_TO_Y. " << std::endl;
	}

	else if 
	(
		(points[0].z == points[1].z)
		&&
		(points[0].z == points[2].z)
	)
	{
		determinedClampValue = PerfectClampEnum::CLAMPED_TO_Z;
		//std::cout << "(ContouredTriangleV2::determinePerfectClampValue()) -> this instance of ContouredTriangleV2 found as being CLAMPED_TO_Z. " << std::endl;
	}
	

	return determinedClampValue;
}