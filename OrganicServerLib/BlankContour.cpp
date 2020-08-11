#include "stdafx.h"
#include "BlankContour.h"

void BlankContour::initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{

}
void BlankContour::amplifyAllContourLinePoints()
{

}

void BlankContour::constructStripTriangles(int in_stripID, int in_materialID)
{


}

void BlankContour::constructBottomStripTriangles(int in_stripID, int in_materialID)
{

}

void BlankContour::constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_polyType)
{
	planDirections.y_direction = -1;	// DEFAULT, can be changed later.

	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry, ECBPolyType::FREE);
	testTriangle.determineLineLengths();
	testTriangle.determineAxisInterceptDistances();
	//std::cout << "!!!!!!!!! Blank contour ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container, testContainer;
		

		//std::cout << "##################### performing new test for the Z of point A of line: " << x << std::endl;
		//testContainer = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//std::cout << "Centroid value is: " << centroid.x << ", " << centroid.y << ", " << centroid.z << std::endl;
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

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

void BlankContour::constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID)
{
	planDirections.y_direction = -1;	// DEFAULT, can be changed later.
	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry, ECBPolyType::FREE);
	testTriangle.determineLineLengths();
	testTriangle.determineAxisInterceptDistances();
	//std::cout << "!!!!!!!!! Blank contour ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container, testContainer;


		//std::cout << "##################### performing new test for the Z of point A of line: " << x << std::endl;
		//testContainer = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

		x_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].x);			// get precise accurate coordinates, relative to blueprint orthodox
		y_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].y);
		z_container = OrganicUtils::getPreciseCoordinate(testTriangle.trianglePoints[x].z);

		ECBPolyPoint centroid = OrganicUtils::determineTriangleCentroid(testTriangle.trianglePoints[0], testTriangle.trianglePoints[1], testTriangle.trianglePoints[2]);
		//std::cout << "Centroid value is: " << centroid.x << ", " << centroid.y << ", " << centroid.z << std::endl;
		//x_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].x, centroid, 0);			// get precise accurate coordinates, relative to blueprint orthodox
		//y_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].y, centroid, 1);
		//z_container = OrganicUtils::getPreciseCoordinateForBlueprint(testTriangle.trianglePoints[x].z, centroid, 2);

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

void BlankContour::runMassDrivers(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{

}