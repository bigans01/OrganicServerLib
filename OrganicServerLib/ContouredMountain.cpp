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
	constructStripTrianglesForLayer(&contourLineMap, &triangleStripMRPMap, in_stripID, in_materialID, &triangleStripMap, ECBPolyType::SHELL, startPoint);
}

void ContouredMountain::constructBottomStripTriangles(int in_stripID, int in_materialID)
{
	std::cout << "Calling BOTTOM strip construction, for strip: " << in_stripID << std::endl;
	std::cout << "!! (PRE)  Current bottom strip map size is: " << bottomTriangleStripMap.size() << std::endl;
	constructStripTrianglesForLayer(&bottomContourLineMap, &triangleBottomStripMRPMap, in_stripID, 1, &bottomTriangleStripMap, ECBPolyType::SHELL_MASSDRIVER, bottomStartPoint);
	std::cout << "!! (POST) Current bottom strip map size is: " << bottomTriangleStripMap.size() << std::endl;
}

void ContouredMountain::constructStripTrianglesForLayer(map<int, OSContourLine>* in_contourLineMapRef, map<int, ECBPolyPoint>* in_triangleStripMRPMapRef, int in_stripID, int in_materialID, unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, ECBPolyType in_type, ECBPolyPoint in_startPoint)
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
			pointThree.x = in_startPoint.x;						// make the center point equal to the "peak" of the contour plan, whatever that may be 
			pointThree.y = in_startPoint.y;
			pointThree.z = in_startPoint.z;

			//constructSingleContouredTriangle(in_startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, in_startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID, in_type);

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
		pointThree.x = in_startPoint.x;
		pointThree.y = in_startPoint.y;
		pointThree.z = in_startPoint.z;

		//constructSingleContouredTriangle(in_startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID);
		contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, in_startPoint, pointOne, pointTwo, massReferencePoint, 0, in_materialID, in_type);

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

		constructOuterQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_type);
		constructOuterQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_type);

		constructInnerQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 0, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 1, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 2, in_stripID, in_materialID, massReferencePoint, in_type);
		constructInnerQuadrantShell(in_osContouredTriangleStripRef,currentLineRef, previousLineRef, pointsPerQuadrantCurrentLine, 3, in_stripID, in_materialID, massReferencePoint, in_type);

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
		//CursorPathTraceContainer x_container, y_container, z_container;
		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox (THIS IS OBSOLETE, DO NOT USE)
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		//x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates
		//y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		//z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);


		EnclaveKeyDef::EnclaveKey blueprintKey;
		//blueprintKey.x = x_container.CollectionCoord;
		//blueprintKey.y = y_container.CollectionCoord;
		//blueprintKey.z = z_container.CollectionCoord;

		blueprintKey.x = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].x);
		blueprintKey.y = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].y);
		blueprintKey.z = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].z);

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
		//CursorPathTraceContainer x_container, y_container, z_container;
		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		//x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		//z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		EnclaveKeyDef::EnclaveKey blueprintKey;
		//blueprintKey.x = x_container.CollectionCoord;
		//blueprintKey.y = y_container.CollectionCoord;
		//blueprintKey.z = z_container.CollectionCoord;

		blueprintKey.x = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].x);
		blueprintKey.y = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].y);
		blueprintKey.z = OrganicUtils::getWorldCoordinateBlueprintDimensionValue(testTriangle.trianglePoints[x].z);

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





void ContouredMountain::constructOuterQuadrantShell(unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type)
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

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

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

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

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

		//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
		contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
	}
}

void ContouredMountain::constructInnerQuadrantShell(unordered_map<int, OSContouredTriangleStrip>* in_osContouredTriangleStripRef, OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp, ECBPolyType in_type)
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

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

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

			//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
			contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

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

		//constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);
		contouredMountainConstructSingleContouredTriangle(in_osContouredTriangleStripRef, firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID, in_type);

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
		std::cout << ">>> Top contour line count is: " << topContourLineCount << std::endl;
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
	std::cout << ">>> Bottom contour line count is: " << bottomContourLineCount << std::endl;
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

void ContouredMountain::runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef)
{
	std::cout << "### Running mass drivers for mountain; printing out SHELL_MASSDSRIVER polys: " << std::endl;
	auto organicstart = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	//EnclaveFractureResultsMap tempMap;		// the tempMap where all the work will be written to.

	// Step 1) find each poly that is a SHELL_MASSDRIVER type, put it into the appropriate ForgedPolySet (mapped by blueprint key). At 
	//			the same time, in the same loop, generate the raw enclave data for each affected blueprint.
	/*
	auto planPolyRegistryBegin = planPolyRegistry.polySetRegistry.begin();
	auto planPolyRegistryEnd = planPolyRegistry.polySetRegistry.end();
	for (planPolyRegistryBegin; planPolyRegistryBegin != planPolyRegistryEnd; planPolyRegistryBegin++)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin->first;					// get the key of the blueprint to check.
		int foundGroupID = planPolyRegistryBegin->second.groupID;									// grab the group ID we'll be working with.
		//std::cout << "Found poly set " << foundGroupID << "in key: (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
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

		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		EnclaveFractureResultsMap tempMap;
		//in_clientRef->OS->produceRawEnclavesForPolySet(in_fractureResultsMapRef, blueprintKey, blueprintToCheck, originalSet.polySet);		// first, generate the OrganicRawEnclaves that would be produced by this set
		//in_clientRef->OS->updateRawEnclaveData(in_fractureResultsMapRef, blueprintToCheck);
		in_clientRef->OS->produceRawEnclavesForPolySet(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet);		// first, generate the OrganicRawEnclaves that would be produced by this set
		in_clientRef->OS->spawnAndAppendEnclaveTriangleSkeletonsToBlueprint(&tempMap, blueprintToCheck);					// second, spawn the EnclaveTriangleSkeletonContainers for the current EnclaveFractureResultsMap; then append the results to the target blueprint to update.

	}
	*/

	
	// Step 1), V2: iterate through the list of affected blueprints, by going through the adherence order; each blueprint in the adherence order (except for the first one) will attempt to perform
	// adherence after the encalves have been produced, but before the triangle data skeletons are appended to the blueprints.
	//auto planPolyRegistryBeginV2 = 
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveFractureResultsMap, EnclaveKeyDef::KeyHasher> containerMapMap;
	auto adherenceListBegin = adherenceData.adherenceOrder.begin();
	auto adherenceListEnd = adherenceData.adherenceOrder.end();
	int currentAdherenceIndex = 0;														// iterate every loop; the very first blueprint doesn't need to do adherence.
	for (; adherenceListBegin != adherenceListEnd; adherenceListBegin++)
	{
		// #############################################################################################################################
		// STEP 1.1): find the SHELL_MASSDRIVER polys that this ContourPlan produced, 
		// which are a subset of the ForgedPolySet, in each blueprint that the ContourPlan touched.

		auto planPolyRegistryBegin = planPolyRegistry.polySetRegistry.find(*adherenceListBegin); // find the forged poly set, for the current blueprint we're looking at
																								 // in the vector. For example, a ForgedPolySet might have a range of polys between 0 and 7 (8 total).
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin->first;					// get the key of the blueprint to check.
		EnclaveCollectionBlueprint* blueprintToCheck = &(*in_blueprintMapRef)[blueprintKey];	// get a ref to the blueprint that exists SERVER side (not on the client), using the blueprintKey
		std::map<int, ECBPoly>* polyMapRef = &blueprintToCheck->primaryPolygonMap;				// get a ref to the poly map inside the blueprint.
		auto forgedPolySetBegin = planPolyRegistryBegin->second.polySet.begin();				// set iterators for the poly set we're using from the planPolyRegistry.
		auto forgedPolySetEnd = planPolyRegistryBegin->second.polySet.end();					// ""
		for (forgedPolySetBegin; forgedPolySetBegin != forgedPolySetEnd; forgedPolySetBegin++)	// check for SHELL_MASSDRIVER polys, and add them appropriately.
		{
			ECBPoly* polyRef = &(*polyMapRef).find(*forgedPolySetBegin)->second;	// get a ref to the poly to find.
			if (polyRef->polyType == ECBPolyType::SHELL_MASSDRIVER)					// if it's a shell mass driver, insert it into the massDriverPolyRegistry
			{
				massDriverPolyRegistry.addToPolyset(blueprintKey, *forgedPolySetBegin);
				//std::cout << "In blueprint, (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << "), polygon with ID: " << *forgedPolySetBegin << std::endl;
			}
		}

		// #############################################################################################################################
		// STEP 1.2): use all the values from each ForgedPolySet -- which represent the IDs of ECBPolys that this ContourPlan added -- for each blueprint, and 
		// then spawn the resulting EnclaveTriangles in the OREs (OrganicRawEnclaves) into the tempMap. When this is done, add the results to the containerMapMap.
		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		EnclaveFractureResultsMap tempMap;
		in_clientRef->OS->produceRawEnclavesForPolySet(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet);		// 1.) Generate the OrganicRawEnclaves that would be produced by this set
		containerMapMap[blueprintKey] = tempMap;																			// 2.) Copy the results, before reunning adherence
		if (currentAdherenceIndex > 0)																						// **the first blueprint never does adherence, as it is the primal blueprint (the first)
		{
			OSServerUtils::runAdherenceForBlueprint(&adherenceData, blueprintKey, &containerMapMap[blueprintKey], &containerMapMap);				// 3.) Run adherence; be sure to pass a ref to the containerMapMap we're working with
		}

		//std::cout << "!!! Spawning and appending skeletons for blueprint: " << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
		in_clientRef->OS->spawnAndAppendEnclaveTriangleSkeletonsToBlueprint(blueprintKey, &containerMapMap[blueprintKey], blueprintToCheck);					// 4.) for each blueprint in the adherence list, 
																																				// spawn the EnclaveTriangleSkeletonContainers from the corresponding EnclaveFractureResultsMap for that blueprint; 
																																				// then append the results to the target blueprint to update.				
		currentAdherenceIndex++;
	}
	


	// Step 2) for each blueprint that contained at least one SHELL_MASSDRIVER, take the original forged poly registry set for that blueprint, and
	//		   subtract the massDriverPolyRegistry set from it to produce a new set. This new set represents the OrganicTriangles that 
	//         will have to be traced in the blueprint that the mass driver begins in (but only for that initial blueprint)
	//std::cout << "Printing unique blueprint keys from massDriverPolyRegistry: " << std::endl;
	auto massDriverRegistryBegin = massDriverPolyRegistry.polySetRegistry.begin();
	auto massDriverRegistryEnd = massDriverPolyRegistry.polySetRegistry.end();
	for (massDriverRegistryBegin; massDriverRegistryBegin != massDriverRegistryEnd; massDriverRegistryBegin++)
	{
		// set up the set to use
		EnclaveKeyDef::EnclaveKey blueprintKey = massDriverRegistryBegin->first;
		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		//ForgedPolySet subtractingSet = massDriverRegistryBegin->second;
		ForgedPolySet massDriverSet = massDriverRegistryBegin->second;				// the set that represents the polys identified as SHELL_MASSDRIVER type.
		ForgedPolySet startingFloorTerminatingSet = originalSet;

		auto subtractionBegin = massDriverSet.polySet.begin();
		auto subtractionEnd = massDriverSet.polySet.end();
		for (subtractionBegin; subtractionBegin != subtractionEnd; subtractionBegin++)
		{
			startingFloorTerminatingSet.polySet.erase(*subtractionBegin);
		}

		std::cout << "Mass Driving blueprint Key: (" << massDriverRegistryBegin->first.x << ", " << massDriverRegistryBegin->first.y << ", " << massDriverRegistryBegin->first.z << ") " << std::endl;
		// now that the shell has been produced in all its affected blueprints, its now time to at last run the mass drivers.
		// a mass driver should pass:

		// 1.) a reference to the blueprint map
		// 2.) a reference to the ContourPlan's planPolyRegistry.polySetRegistry
		// 3.) the blueprint key of the blueprint that the mass driver starts in
		// 4.) the "newSet" that represents all polygons that a SHELL_MASSDRIVER polygon type may or may NOT hit
		// 5.) the "subtractingSet" that represents the SHELL_MASSDRIVER polygons
		//
		// The new function will need to produce a "phantom" blueprint that contains only the polygons in the originalSet, to make it seem
		// like those are the only ones that exist. To do this, the "originalSet" is used to trace 
		in_clientRef->OS->generateAndRunMassDriversForBlueprint(in_blueprintMapRef, &planPolyRegistry.polySetRegistry, blueprintKey, startingFloorTerminatingSet, massDriverSet);

		//std::cout << "OrganicRawPolys updated... " << std::endl;
	}

	auto organicend = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	std::chrono::duration<double> organicelapsed = organicend - organicstart;

	std::cout << "Size of map map is: " << containerMapMap.size() << std::endl;
	std::cout << "### End of mass driver run. Time spent:" << organicelapsed.count() << std::endl;
	std::cout << "### ContouredMountain summit was:  " << startPoint.x << ", " << startPoint.y << ", " << startPoint.z << std::endl;

	//int someVal = 3;
	//std::cin >> someVal;
}
