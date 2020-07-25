#include "stdafx.h"
#include "ContouredMountain.h"

void ContouredMountain::initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	planDirections.y_direction = -1;	// for OSPDir::BELOW (MOUNTAIN)
	setFormationBaseContourPoints(in_startPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);
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

	}
}

void ContouredMountain::constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID)
{
	/*
	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	testPoint_0.x = -42.0f;
	testPoint_0.y = 2.4f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 2.0f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = 10.0f;
	testPoint_1.z = 10.0f;

	testPoint_2.x = -8.3f;
	testPoint_2.y = 2.0f;
	testPoint_2.z = 2.0f;
	*/
	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry);

	/*
	OSContouredTriangle testTriangle;
	testTriangle.trianglePoints[0] = in_point0;
	testTriangle.trianglePoints[1] = in_point1;
	testTriangle.trianglePoints[2] = in_point2;
	testTriangle.materialID = in_materialID;		// set the material of the triangle
	testTriangle.massReferencePoint = in_massReferencePoint;
	*/
	testTriangle.determineLineLengths();
	/*
	if (testTriangle.trianglePoints[0].x > 218.0f)
	{

		testTriangle.determineAxisInterceptDistancesDebug();
	}
	else
	{
		testTriangle.determineAxisInterceptDistances();
	}
	*/
	testTriangle.determineAxisInterceptDistances();
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		//x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		//z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;

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

void ContouredMountain::amplifyContourLinePoints(int in_lineID)
{
	contourLineMap[in_lineID].amplifyAllPoints();
}





void ContouredMountain::constructOuterQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp)
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

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID);

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

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID);

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

		constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID);
	}
}

void ContouredMountain::constructInnerQuadrantShell(OSContourLine* in_currentLine, OSContourLine* in_previousLine, int in_pointsPerQuadrant, int in_quadrantID, int in_triangleStripID, int in_materialID, ECBPolyPoint in_mrp)
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

			constructSingleContouredTriangle(firstPoint, secondPoint, thirdPoint, in_mrp, in_triangleStripID, in_materialID);

			basePointForPreviousLine++;		// iterate the base points
			basePointForCurrentLine++;		// ""
		}


	}

}

void ContouredMountain::setFormationBaseContourPoints(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{

		// set up the first contour line
		/*
		float currentYElevation = in_startPoint.y - in_distanceBetweenLayers;
		addContourLine(contourLineCount, in_startRadius, currentYElevation, 4, in_startPoint);
		amplifyContourLinePoints(0);
		std::cout << "Preparing cycle..." << std::endl;
		for (int x = 0; x < 4; x++)
		{
			std::cout << x << ": " << "(" << contourLineMap[0].smartContourPoint[x].x << ", " << contourLineMap[0].smartContourPoint[x].y << ", " << contourLineMap[0].smartContourPoint[x].z << ") " << std::endl;
		}
		*/
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
			insertMRP(x, mrpToAdd);					// insert the MRP
			addContourLine(contourLineCount, currentRadius, currentY, currentNumberOfPoints, in_startPoint);

		}
		
}

void ContouredMountain::insertMRP(int in_stripValue, ECBPolyPoint in_polyPoint)
{
	triangleStripMRPMap[in_stripValue] = in_polyPoint;	// insert the MRP for this strip
}

void ContouredMountain::addContourLine(int line_id, float in_baseDistance, float in_contourElevation, int in_numberOfPoints, ECBPolyPoint in_startPoint)	// adds a contour line, with the map ID of the line equal to contourLineCoun
{
	OSContourLine tempLine(in_baseDistance, in_contourElevation, in_numberOfPoints, in_startPoint);
	contourLineMap[line_id] = tempLine;
	contourLineCount++;
}

void ContouredMountain::runMassDrivers()
{

}