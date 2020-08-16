#include "stdafx.h"
#include "ContouredMountain.h"

void ContouredMountain::initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	planDirections.y_direction = -1;	// for OSPDir::BELOW (MOUNTAIN)
	numberOfLayers = in_numberOfLayers;
	setFormationBaseContourPoints(in_startPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
	setMRPsForBottomLayers();
	setFormationBottomBaseContourPoints(triangleBottomStripMRPMap[0], in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
}

void ContouredMountain::amplifyAllContourLinePoints()
{
	int numberOfLayers = contourLineMap.size();
	for (int x = 0; x < numberOfLayers; x++)
	{
		amplifyContourLinePoints(x);
	}
}

void ContouredMountain::constructStripTriangles(int in_stripID, int in_materialID)
{
	/*
	if (in_stripID == 0)
	{
		OSContourLine* firstLineRef = &contourLineMap[0];
		int numberOfPoints = firstLineRef->numberOfPoints;
		ECBPolyPoint currentContourCenter = firstLineRef->centerPoint;
		//std::cout << "MOUNTAIN: number of points: " << numberOfPoints << std::endl;
		std::cout << "+++++++++++++++++Contour line center point is: " << currentContourCenter.x << ", " << currentContourCenter.y << ", " << currentContourCenter.z << ", " << std::endl;
		ECBPolyPoint massReferencePoint = triangleStripMRPMap[in_stripID];		// grab the MRP
		std::cout << "+++++++++++++++++++++MRP is: " << massReferencePoint.x << ", " << massReferencePoint.y << ", " << massReferencePoint.z << std::endl;
		//int someDumbVal = 3; 
		//std::cin >> someDumbVal;

		for (int x = 0; x < numberOfPoints - 1; x++)
		{
			//OSContourPoint* contourPointPointer = &PointMap[in_pointIndex];

			// first triangle point
			OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[x];
			ECBPolyPoint pointOne;
			pointOne.x = contourPointPointer->x;
			pointOne.y = contourPointPointer->y;
			pointOne.z = contourPointPointer->z;

			//second triangle point
			contourPointPointer = &firstLineRef->smartContourPoint[x + 1];
			ECBPolyPoint pointTwo;
			pointTwo.x = contourPointPointer->x;
			pointTwo.y = contourPointPointer->y;
			pointTwo.z = contourPointPointer->z;

			//third triangle point
			ECBPolyPoint pointThree;
			pointThree.x = startPoint.x;						// make the center point equal to the "peak" of the contour plan, whatever that may be 
			pointThree.y = startPoint.y;
			pointThree.z = startPoint.z;

			constructSingleContouredTriangle(startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);

			//std::cout << "Current Points: " << pointOne.x << ", " << pointOne.y << ", " << pointOne.z << std::endl;
		}
		// do the following for the last triangle only
		int finalPointOne = numberOfPoints - 1;

		OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[finalPointOne];
		ECBPolyPoint pointOne;
		pointOne.x = contourPointPointer->x;
		pointOne.y = contourPointPointer->y;
		pointOne.z = contourPointPointer->z;

		contourPointPointer = &firstLineRef->smartContourPoint[0];
		ECBPolyPoint pointTwo;
		pointTwo.x = contourPointPointer->x;
		pointTwo.y = contourPointPointer->y;
		pointTwo.z = contourPointPointer->z;

		ECBPolyPoint pointThree;
		pointThree.x = startPoint.x;
		pointThree.y = startPoint.y;
		pointThree.z = startPoint.z;

		constructSingleContouredTriangle(startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);

		//std::cout << "Current Points: " << pointOne.x << ", " << pointOne.y << ", " << pointOne.z << std::endl;
		//std::cout << "First layer triangle created, via new function call...." << endl;
	}
	else
	{
		OSContourLine* currentLineRef = &contourLineMap[in_stripID];		// a reference to the current line
		OSContourLine* previousLineRef = &contourLineMap[in_stripID - 1];	// a reference to the previous line

		int numberOfPoints = currentLineRef->numberOfPoints;
		ECBPolyPoint currentContourCenter = currentLineRef->centerPoint;
		std::cout << "+++++++++++++++++Contour line center point is: " << currentContourCenter.x << ", " << currentContourCenter.y << ", " << currentContourCenter.z << ", " << std::endl;
		ECBPolyPoint massReferencePoint = triangleStripMRPMap[in_stripID];		// grab the MRP
		std::cout << "+++++++++++++++++++++MRP is: " << massReferencePoint.x << ", " << massReferencePoint.y << ", " << massReferencePoint.z << std::endl;
		//std::cout << "Number of points on this contour line is: " << currentLineRef->numberOfPoints << std::endl;

		int pointsPerQuadrantCurrentLine = (numberOfPoints / 4) + 1;	// get the number of points per quadrant,  for the current line
		int trianglesForCurrentLine = pointsPerQuadrantCurrentLine - 1;	// the number of triangles in each quadrant is equal to the number of points per quadrant - 1

		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint);

		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint);

	}
	*/
	constructStripTrianglesForLayer(&contourLineMap, &triangleStripMRPMap, in_stripID, in_materialID, &triangleStripMap, ECBPolyType::SHELL);
}

void ContouredMountain::constructBottomStripTriangles(int in_stripID, int in_materialID)
{
	constructStripTrianglesForLayer(&bottomContourLineMap, &triangleBottomStripMRPMap, in_stripID, 1, &bottomTriangleStripMap, ECBPolyType::SHELL_MASSDRIVER);
}

void ContouredMountain::constructStripTrianglesForLayer(map<int, OSContourLine>* in_contourLineMapRef, map<int, ECBPolyPoint>* in_triangleStripMRPMapRef, int in_stripID, int in_materialID, unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, ECBPolyType in_type)
{
	if (in_stripID == 0)
	{
		OSContourLine* firstLineRef = &(*in_contourLineMapRef)[0];
		int numberOfPoints = firstLineRef->numberOfPoints;
		ECBPolyPoint currentContourCenter = firstLineRef->centerPoint;
		//std::cout << "MOUNTAIN: number of points: " << numberOfPoints << std::endl;
		std::cout << "+++++++++++++++++Contour line center point is: " << currentContourCenter.x << ", " << currentContourCenter.y << ", " << currentContourCenter.z << ", " << std::endl;
		ECBPolyPoint massReferencePoint = (*in_triangleStripMRPMapRef)[in_stripID];		// grab the MRP
		std::cout << "+++++++++++++++++++++MRP is: " << massReferencePoint.x << ", " << massReferencePoint.y << ", " << massReferencePoint.z << std::endl;
		//int someDumbVal = 3; 
		//std::cin >> someDumbVal;

		for (int x = 0; x < numberOfPoints - 1; x++)
		{
			//OSContourPoint* contourPointPointer = &PointMap[in_pointIndex];

			// first triangle point
			OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[x];
			ECBPolyPoint pointOne;
			pointOne.x = contourPointPointer->x;
			pointOne.y = contourPointPointer->y;
			pointOne.z = contourPointPointer->z;

			//second triangle point
			contourPointPointer = &firstLineRef->smartContourPoint[x + 1];
			ECBPolyPoint pointTwo;
			pointTwo.x = contourPointPointer->x;
			pointTwo.y = contourPointPointer->y;
			pointTwo.z = contourPointPointer->z;

			//third triangle point
			ECBPolyPoint pointThree;
			pointThree.x = startPoint.x;						// make the center point equal to the "peak" of the contour plan, whatever that may be 
			pointThree.y = startPoint.y;
			pointThree.z = startPoint.z;

			//constructSingleContouredTriangle(startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID, in_type);

			std::cout << "Current Points: " << pointOne.x << ", " << pointOne.y << ", " << pointOne.z << std::endl;
		}
		// do the following for the last triangle only
		int finalPointOne = numberOfPoints - 1;

		OSContourPoint* contourPointPointer = &firstLineRef->smartContourPoint[finalPointOne];
		ECBPolyPoint pointOne;
		pointOne.x = contourPointPointer->x;
		pointOne.y = contourPointPointer->y;
		pointOne.z = contourPointPointer->z;

		contourPointPointer = &firstLineRef->smartContourPoint[0];
		ECBPolyPoint pointTwo;
		pointTwo.x = contourPointPointer->x;
		pointTwo.y = contourPointPointer->y;
		pointTwo.z = contourPointPointer->z;

		ECBPolyPoint pointThree;
		pointThree.x = startPoint.x;
		pointThree.y = startPoint.y;
		pointThree.z = startPoint.z;

		//constructSingleContouredTriangle(startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);
		contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID, in_type);

		//std::cout << "Current Points: " << pointOne.x << ", " << pointOne.y << ", " << pointOne.z << std::endl;
		std::cout << "First layer triangle created, via new function call...." << endl;
	}
	else
	{
		OSContourLine* currentLineRef = &(*in_contourLineMapRef)[in_stripID];		// a reference to the current line
		OSContourLine* previousLineRef = &(*in_contourLineMapRef)[in_stripID - 1];	// a reference to the previous line

		int numberOfPoints = currentLineRef->numberOfPoints;
		ECBPolyPoint currentContourCenter = currentLineRef->centerPoint;
		std::cout << "+++++++++++++++++Contour line center point is: " << currentContourCenter.x << ", " << currentContourCenter.y << ", " << currentContourCenter.z << ", " << std::endl;
		ECBPolyPoint massReferencePoint = (*in_triangleStripMRPMapRef)[in_stripID];		// grab the MRP
		std::cout << "+++++++++++++++++++++MRP is: " << massReferencePoint.x << ", " << massReferencePoint.y << ", " << massReferencePoint.z << std::endl;
		//std::cout << "Number of points on this contour line is: " << currentLineRef->numberOfPoints << std::endl;

		int pointsPerQuadrantCurrentLine = (numberOfPoints / 4) + 1;	// get the number of points per quadrant,  for the current line
		int trianglesForCurrentLine = pointsPerQuadrantCurrentLine - 1;	// the number of triangles in each quadrant is equal to the number of points per quadrant - 1

		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_type);

		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_type);

	}
}


void ContouredMountain::setMRPsForBottomLayers()
{
	// check for special bottom MRP logic
	if (numberOfLayers == 1)
	{
		triangleBottomStripMRPMap[0] = startPoint;	// if we only have one layer on the top, the MRP of the bottom strip should be equal to the start point.
	}
	else
	{
		triangleBottomStripMRPMap[0] = triangleStripMRPMap.rbegin()->second;	// otherwise, the MRP of all the bottom strips can just be the last MRP in the first strip.
	}
}

void ContouredMountain::constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_type)
{
	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry, in_type);
	testTriangle.determineLineLengths();
	testTriangle.determineAxisInterceptDistances();
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);


		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

		blueprintKey.x = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].x);
		blueprintKey.y = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].y);
		blueprintKey.z = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].z);

		//currentTriPoint.triPoints[x] = testTriangle.trianglePoints[x];		// add this point and its assumed precise blueprint key
		//currentTriKey.triKey[x] = blueprintKey;
		testTriangle.pointKeys[x] = blueprintKey;
		testTriangle.centroid = centroid;
	}
	int baseStripSize = triangleStripMap[in_triangleStripID].triangleMap.size();		// get the number of triangles in the base strip, should be 0
	//std::cout << "### Adding new triangle with ID " << baseStripSize << std::endl;
	triangleStripMap[in_triangleStripID].triangleMap[baseStripSize] = testTriangle;
	//std::cout << "### New size is: " << triangleStripMap[in_triangleStripID].triangleMap.size() << std::endl;
}

void ContouredMountain::constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID)
{

}

void ContouredMountain::contouredMountainConstructSingleContouredTriangle(unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_type)
{
	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry, in_type);
	testTriangle.determineLineLengths();
	testTriangle.determineAxisInterceptDistances();
	testTriangle.determineAxisInterceptDistances();
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

		blueprintKey.x = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].x);
		blueprintKey.y = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].y);
		blueprintKey.z = OrganicUtils::getPreciseCoordinateForBlueprintV2(testTriangle.trianglePoints[x].z);

		//currentTriPoint.triPoints[x] = testTriangle.trianglePoints[x];		// add this point and its assumed precise blueprint key
		//currentTriKey.triKey[x] = blueprintKey;
		testTriangle.pointKeys[x] = blueprintKey;
		testTriangle.centroid = centroid;
	}
	int baseStripSize = (*in_osContouredTriangleStripRef)[in_triangleStripID].triangleMap.size();		// get the number of triangles in the base strip, should be 0
	//std::cout << "### Adding new triangle with ID " << baseStripSize << std::endl;
	(*in_osContouredTriangleStripRef)[in_triangleStripID].triangleMap[baseStripSize] = testTriangle;
	//std::cout << "### New size is: " << (*in_osContouredTriangleStripRef)[in_triangleStripID].triangleMap.size() << std::endl;

}

void ContouredMountain::amplifyContourLinePoints(int in_lineID)
{
	contourLineMap[in_lineID].amplifyAllPoints();
	bottomContourLineMap[in_lineID].amplifyAllPoints();
}





void ContouredMountain::constructOuterQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type)
{
	if (in_quadrantID != 3)		// don't do this for the last quadrant
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1));		// get the point ID on the current line to start at, which is based on the quadrant id
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 1);			// the number of triangles to produce in the outer shell

		// produce the triangles

		for (int x = 0; x < numberOfTriangles; x++)
		{
			//std::cout << "!!!!!!!!!!!!!!!!!!!!! ->>>>>>>>> constructing quadrant triangle " << std::endl;
			

			OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
			ECBPolyPoint firstPoint;
			firstPoint.x = firstContourPoint->x;
			firstPoint.y = firstContourPoint->y;
			firstPoint.z = firstContourPoint->z;

			OSContourPoint* secondContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];		// get the second point from the current line
			ECBPolyPoint secondPoint;
			secondPoint.x = secondContourPoint->x;
			secondPoint.y = secondContourPoint->y;
			secondPoint.z = secondContourPoint->z;

			OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine + 1];	// get the third point from the current line
			ECBPolyPoint thirdPoint;
			thirdPoint.x = thirdContourPoint->x;
			thirdPoint.y = thirdContourPoint->y;
			thirdPoint.z = thirdContourPoint->z;

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""

		}
	}

	else if (in_quadrantID == 3)	// special case for the last quadrant
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1));		// get the point ID on the current line to start at, which is based on the quadrant id
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfNormalTriangles = (in_pointsPerQuadrant - 1) - 1;	// we need special logic for the very last triangle
		for (int x = 0; x < numberOfNormalTriangles; x++)
		{
			//std::cout << "!!!!!!!!!!!!!!!!!!!!! ->>>>>>>>> constructing FINAL quadrant triangle " << std::endl;

			OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
			ECBPolyPoint firstPoint;
			firstPoint.x = firstContourPoint->x;
			firstPoint.y = firstContourPoint->y;
			firstPoint.z = firstContourPoint->z;

			OSContourPoint* secondContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];		// get the second point from the current line
			ECBPolyPoint secondPoint;
			secondPoint.x = secondContourPoint->x;
			secondPoint.y = secondContourPoint->y;
			secondPoint.z = secondContourPoint->z;

			OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine + 1];	// get the third point from the current line
			ECBPolyPoint thirdPoint;
			thirdPoint.x = thirdContourPoint->x;
			thirdPoint.y = thirdContourPoint->y;
			thirdPoint.z = thirdContourPoint->z;

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}

		// perform logic for final triangle
		basePointForPreviousLine = 0;
		OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
		ECBPolyPoint firstPoint;
		firstPoint.x = firstContourPoint->x;
		firstPoint.y = firstContourPoint->y;
		firstPoint.z = firstContourPoint->z;

		OSContourPoint* secondContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];		// get the second point from the current line
		ECBPolyPoint secondPoint;
		secondPoint.x = secondContourPoint->x;
		secondPoint.y = secondContourPoint->y;
		secondPoint.z = secondContourPoint->z;

		OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[0];	// get the third point from the current line
		ECBPolyPoint thirdPoint;
		thirdPoint.x = thirdContourPoint->x;
		thirdPoint.y = thirdContourPoint->y;
		thirdPoint.z = thirdContourPoint->z;

		constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
	}
}

void ContouredMountain::constructInnerQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type)
{
	if (in_quadrantID != 3)		// don't do this for the last quadrant
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1)) + 1;		// get the point ID on the current line to start at, which is based on the quadrant id; because this is an inner shell, we increment base point by 1
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 2);			// the number of triangles to produce in the inner shell; will be equal to points per quadrant - 2.

		for (int x = 0; x < numberOfTriangles; x++)
		{
			OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
			ECBPolyPoint firstPoint;
			firstPoint.x = firstContourPoint->x;
			firstPoint.y = firstContourPoint->y;
			firstPoint.z = firstContourPoint->z;

			OSContourPoint* secondContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine + 1];		// get the second point from the current line
			ECBPolyPoint secondPoint;
			secondPoint.x = secondContourPoint->x;
			secondPoint.y = secondContourPoint->y;
			secondPoint.z = secondContourPoint->z;

			OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];	// get the third point from the current line
			ECBPolyPoint thirdPoint;
			thirdPoint.x = thirdContourPoint->x;
			thirdPoint.y = thirdContourPoint->y;
			thirdPoint.z = thirdContourPoint->z;

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}


	}

	
	else if (in_quadrantID == 3)
	{
		int basePointForCurrentLine = (in_quadrantID * (in_pointsPerQuadrant - 1)) + 1;		// get the point ID on the current line to start at, which is based on the quadrant id; because this is an inner shell, we increment base point by 1
		int basePointForPreviousLine = (in_quadrantID * (in_pointsPerQuadrant - 2));	// get the point ID on the previous line to start at, which is based on the quadrant ID
		int numberOfTriangles = (in_pointsPerQuadrant - 2) - 1;			// the number of triangles to produce in the inner shell; will be equal to points per quadrant - 2.

		
		for (int x = 0; x < numberOfTriangles; x++)
		{
			OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
			ECBPolyPoint firstPoint;
			firstPoint.x = firstContourPoint->x;
			firstPoint.y = firstContourPoint->y;
			firstPoint.z = firstContourPoint->z;

			OSContourPoint* secondContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine + 1];		// get the second point from the current line
			ECBPolyPoint secondPoint;
			secondPoint.x = secondContourPoint->x;
			secondPoint.y = secondContourPoint->y;
			secondPoint.z = secondContourPoint->z;

			OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];	// get the third point from the current line
			ECBPolyPoint thirdPoint;
			thirdPoint.x = thirdContourPoint->x;
			thirdPoint.y = thirdContourPoint->y;
			thirdPoint.z = thirdContourPoint->z;

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}
		

		// perform logic for final triangle
		OSContourPoint* firstContourPoint = &in_previousLine->smartContourPoint[basePointForPreviousLine];		// get the first point from the previous line
		ECBPolyPoint firstPoint;
		firstPoint.x = firstContourPoint->x;
		firstPoint.y = firstContourPoint->y;
		firstPoint.z = firstContourPoint->z;

		OSContourPoint* secondContourPoint = &in_previousLine->smartContourPoint[0];		// get the second point from the current line
		ECBPolyPoint secondPoint;
		secondPoint.x = secondContourPoint->x;
		secondPoint.y = secondContourPoint->y;
		secondPoint.z = secondContourPoint->z;

		OSContourPoint* thirdContourPoint = &in_currentLine->smartContourPoint[basePointForCurrentLine];	// get the third point from the current line
		ECBPolyPoint thirdPoint;
		thirdPoint.x = thirdContourPoint->x;
		thirdPoint.y = thirdContourPoint->y;
		thirdPoint.z = thirdContourPoint->z;

		constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

	}
	
	
}

void ContouredMountain::setFormationBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
		startPoint = in_startPoint;
		float currentY = in_startPoint.y;
		int currentNumberOfPoints = 4;				// first layer always starts with 4 points (for now, this will probably change later)
		float currentRadius = in_startRadius;		// set the start radius
		for (int x = 0; x < in_numberOfLayers; x++)
		{
			currentY -= in_distanceBetweenLayers;	// subtract the distance between layers for each layer being added
			ECBPolyPoint mrpToAdd;					// set up the MRP to insert for this triangle strip
			mrpToAdd = startPoint;					// "" 
			mrpToAdd.y = currentY;					// ""
			if (x != 0)								// it isn't necessary to add on to the radius for the very first strip
			{
				currentRadius += in_expansionValue;
				currentNumberOfPoints += 4;
			}
			insertMRP(&triangleStripMRPMap, &x, mrpToAdd);					// insert the MRP
			addContourLine(&contourLineMap, &topContourLineCount, currentRadius, currentY, currentNumberOfPoints, in_startPoint);

		}
		
}

void ContouredMountain::setFormationBottomBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	bottomStartPoint = in_startPoint;
	int currentNumberOfPoints = 4;
	float currentRadius = in_startRadius;		// set the start radius
	ECBPolyPoint bottomLayerMRP = triangleBottomStripMRPMap[0];
	for (int x = 0; x < in_numberOfLayers; x++)
	{
		if (x != 0)								// it isn't necessary to add on to the radius for the very first strip
		{
			currentRadius += in_expansionValue;
			currentNumberOfPoints += 4;
		}
		insertMRP(&triangleBottomStripMRPMap, &x, bottomLayerMRP);					// insert the MRP
		addContourLine(&bottomContourLineMap, &bottomContourLineCount, currentRadius, in_startPoint.y, currentNumberOfPoints, in_startPoint);
	}
}

void ContouredMountain::insertMRP(map<int, ECBPolyPoint>* in_mrpMapRef, int* in_stripValue, ECBPolyPoint in_polyPoint)
{
	(*in_mrpMapRef)[*in_stripValue] = in_polyPoint;	// insert the MRP for this strip
}

void ContouredMountain::addContourLine(map<int, OSContourLine>* in_contourLineMapRef, int* line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint)	// adds a contour line, with the map ID of the line equal to contourLineCoun
{
	OSContourLine tempLine(in_baseDistance, in_contourElevation, in_numberOfPoints, in_startPoint);
	(*in_contourLineMapRef)[*line_id] = tempLine;
	(*line_id)++;									
}

void ContouredMountain::runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{
	std::cout << "### Running mass drivers for mountain; printing out SHELL_MASSDSRIVER polys: " << std::endl;

	// Step 1) find each poly that is a SHELL_MASSDRIVER type, put it into the appropriate ForgedPolySet (mapped by blueprint key).
	auto planPolyRegistryBegin = planPolyRegistry.polySetRegistry.begin();
	auto planPolyRegistryEnd = planPolyRegistry.polySetRegistry.end();
	for (planPolyRegistryBegin; planPolyRegistryBegin != planPolyRegistryEnd; planPolyRegistryBegin++)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin->first;					// get the key of the blueprint to check.
		int foundGroupID = planPolyRegistryBegin->second.groupID;									// grab the group ID we'll be working with.
		std::cout << "Found poly set " << foundGroupID << "in key: (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
		EnclaveCollectionBlueprint* blueprintToCheck = &(*in_blueprintMapRef)[blueprintKey];	// get a ref to the blueprint that exists SERVER side (not on the client), using the blueprintKey
		std::map<int, ECBPoly>* polyMapRef = &blueprintToCheck->primaryPolygonMap;				// get a ref to the poly map inside the blueprint.
		auto forgedPolySetBegin = planPolyRegistryBegin->second.polySet.begin();
		auto forgedPolySetEnd = planPolyRegistryBegin->second.polySet.end();
		for (forgedPolySetBegin; forgedPolySetBegin != forgedPolySetEnd; forgedPolySetBegin++)
		{
			ECBPoly* polyRef = &(*polyMapRef).find(*forgedPolySetBegin)->second;	// get a ref to the poly to find.
			if (polyRef->polyType == ECBPolyType::SHELL_MASSDRIVER)					// if it's a shell mass driver, insert it into the massDriverPolyRegistry
			{
				massDriverPolyRegistry.addToPolyset(blueprintKey, *forgedPolySetBegin);
				//std::cout << "In blueprint, (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << "), polygon with ID: " << *forgedPolySetBegin << std::endl;
			}
		}

		//ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		//EnclaveFractureResultsMap tempMap;
		//in_clientRef->OS->produceRawEnclavesForPolySet(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet);		// first, generate the OrganicRawEnclaves that would be produced by this set
		//in_clientRef->OS->updateRawEnclaveData(&tempMap, blueprintToCheck);
	}

	// Step 2) for each blueprint that contained at least one SHELL_MASSDRIVER, take the original forged poly registry set for that blueprint, and
	//		   subtract the massDriverPolyRegistry set from it to produce a new set. This new set represents the OrganicTriangles that 
	//         will have to be traced in the blueprint that the mass driver begins in (but only for that initial blueprint)
	auto massDriverBlueprintKeysBegin = massDriverPolyRegistry.polySetRegistry.begin();
	auto massDriverBlueprintKeysEnd = massDriverPolyRegistry.polySetRegistry.end();
	for (massDriverBlueprintKeysBegin; massDriverBlueprintKeysBegin != massDriverBlueprintKeysEnd; massDriverBlueprintKeysBegin++)
	{
	
		EnclaveKeyDef::EnclaveKey bindingKey = massDriverBlueprintKeysBegin->first;	// get the key
		std::cout << "#### Performing set subtraction for Blueprint: (" << bindingKey.x << ", " << bindingKey.y << ", " << bindingKey.z << ")" << std::endl;

		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[bindingKey];	// get the original, unaltered set
		// print original set values
		/*
		std::cout << "Original set values: " << std::endl;
		auto originalSetBegin = originalSet.polySet.begin();
		auto originalSetEnd = originalSet.polySet.end();
		for (originalSetBegin; originalSetBegin != originalSetEnd; originalSetBegin++)
		{
			std::cout << *originalSetBegin << std::endl;
		}
		*/

		ForgedPolySet subtractingSet = massDriverBlueprintKeysBegin->second;		// get the set corresponding to this blueprint that represents the SHELL_MASSDRIVERS
		/*
		std::cout << "Subtracting set values: " << std::endl;
		auto subtractingSetBegin = subtractingSet.polySet.begin();
		auto subtractingSetEnd = subtractingSet.polySet.end();
		for (subtractingSetBegin; subtractingSetBegin != subtractingSetEnd; subtractingSetBegin++)
		{
			std::cout << *subtractingSetBegin << std::endl;
		}
		*/

		ForgedPolySet newSet = originalSet;
		auto subtractionBegin = subtractingSet.polySet.begin();
		auto subtractionEnd = subtractingSet.polySet.end();
		for (subtractionBegin; subtractionBegin != subtractionEnd; subtractionBegin++)
		{
			newSet.polySet.erase(*subtractionBegin);
		}

		/*
		std::cout << "Resulting set values: " << std::endl;
		auto newSetBegin = newSet.polySet.begin();
		auto newSetEnd = newSet.polySet.end();
		for (newSetBegin; newSetBegin != newSetEnd; newSetBegin++)
		{
			std::cout << *newSetBegin << std::endl;
		}
		*/
		//std::set_difference(originalSet.polySet.begin(), originalSet.polySet.end(), subtractingSet.polySet.begin(), subtractingSet.polySet.end(), newSet.begin());
	}

	// Step 3)
	std::cout << "Printing unique blueprint keys from massDriverPolyRegistry: " << std::endl;
	auto massDriverRegistryBegin = massDriverPolyRegistry.polySetRegistry.begin();
	auto massDriverRegistryEnd = massDriverPolyRegistry.polySetRegistry.end();
	for (massDriverRegistryBegin; massDriverRegistryBegin != massDriverRegistryEnd; massDriverRegistryBegin++)
	{
		// set up the set to use

		//std::cout << "Debug (1) " << std::endl;
		EnclaveKeyDef::EnclaveKey blueprintKey = massDriverRegistryBegin->first;
		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		//std::cout << "Debug (2 1) " << std::endl;
		ForgedPolySet subtractingSet = massDriverRegistryBegin->second;
		//std::cout << "Debug (2 2) " << std::endl;
		ForgedPolySet newSet = originalSet;
		//std::cout << "Debug (2 3) " << std::endl;
		auto subtractionBegin = subtractingSet.polySet.begin();
		auto subtractionEnd = subtractingSet.polySet.end();

		//std::cout << "Debug (3) " << std::endl;
		for (subtractionBegin; subtractionBegin != subtractionEnd; subtractionBegin++)
		{
			newSet.polySet.erase(*subtractionBegin);
		}

		std::cout << "Key: (" << massDriverRegistryBegin->first.x << ", " << massDriverRegistryBegin->first.y << ", " << massDriverRegistryBegin->first.z << ") " << std::endl;
		EnclaveFractureResultsMap tempMap;
		EnclaveCollectionBlueprint* blueprintToCheck = &(*in_blueprintMapRef)[blueprintKey];
		in_clientRef->OS->produceRawEnclavesForPolySet(&tempMap, blueprintKey, blueprintToCheck, newSet.polySet);		// first, generate the OrganicRawEnclaves that would be produced by this set
		in_clientRef->OS->updateRawEnclaveData(&tempMap, blueprintToCheck);												// now, update the blueprint we are modifying, by updating it with the 
																														// OrganicRawEnclaves we created in tempMap or adding them if they don't exist yet.

		//std::cout << "OrganicRawPolys updated... " << std::endl;
	}

	std::cout << "### End of mass driver run. " << std::endl;
	std::cout << "### ContouredMountain summit was:  " << startPoint.x << ", " << startPoint.y << ", " << startPoint.z << std::endl;

	int someVal = 3;
	std::cin >> someVal;
}
