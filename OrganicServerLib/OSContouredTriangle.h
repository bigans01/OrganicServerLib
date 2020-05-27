#pragma once

#ifndef OSCONTOUREDTRIANGLE_H
#define OSCONTOUREDTRIANGLE_H

#include "OSTrianglePoint.h"
#include "EnclaveKeyDef.h"
#include "TriangleLine.h"
#include "ECBBorderValues.h"
#include "PolyUtils.h"
#include "PrimarySegmentTracker.h"
#include "ECBPoly.h"
#include <unordered_map>
#include <math.h>
#include "PrimaryCircuit.h"
#include "ForgedPolyRegistry.h"
#include "EmptyNormalFinder.h"
#include <glm/glm.hpp>

//#include <unordered_set>

class OSContouredTriangle {
public:
	ECBPolyPoint trianglePoints[3];				// stores 3 points
	EnclaveKeyDef::EnclaveKey pointKeys[3];		// stores the keys of the 3 points
	TriangleLine triangleLines[3];			// stores 3 lines
	ECBPolyPoint massReferencePoint;			// mass reference point that will be copied to all ECBPolys generated by this contoured triangle.
	ECBPolyPoint contouredEmptyNormal;		// stores the empty normal that will be passed on to polys spawned by this contoured triangle; should be calculated during the OSContouredTriangle's instantiation/construction
	ForgedPolyRegistry* forgedPolyRegistryRef;	// a reference to the forged poly registry that belongs to the parent contour plan (which spawns this contoured triangle)
	ECBPolyPoint centroid;					// stores the centroid of the triangle.

	OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2);
	OSContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, int in_materialID, ECBPolyPoint in_massReferencePoint, ForgedPolyRegistry* in_forgedPolyRegistryRef);
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> polygonPieceMap;			// stores the ID of the corresponding ECBPolygon(s) related to this instance of OSContouredTriangle, the ID of which is found in each blueprint.
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ECBBorderValues, EnclaveKeyDef::KeyHasher> ecbBorderMap;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> tracedBlueprintCountMap;	// stores blueprint IDs that are a result of tracing the contour lines
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> filledBlueprintMap;		// stores blueprint IDs that were filled 
	std::unordered_map<EnclaveKeyDef::EnclaveKey, PrimaryCircuit, EnclaveKeyDef::KeyHasher> primarySegmentTrackerMap;
	OSContouredTriangle();
	short isTriangleFlat = 0;				// set to 1 if the Triangle is flat (all points on same y level)
	short materialID = 0;				// what is the material made of? stone, brick, snow, dirt, etc?
	int perfect_clamp_x = 0;			// are all 3 points of this triangle perfectly on x axis?
	int perfect_clamp_y = 0;			// ...y axis?
	int perfect_clamp_z = 0;			// ...z axis?
	void addPolygonPiece(EnclaveKeyDef::EnclaveKey in_Key, int in_pieceType);
	void determineLineLengths();		// gets the length of lines
	void determineAxisInterceptDistances();
	void determineAxisInterceptDistancesDebug();
	//void determineLineAxisIntercept(TriangleLine* in_LinePtr, ECBPolyPoint in_thirdPoint);
	//void determineLineAxisInterceptDebug(TriangleLine* in_LinePtr, ECBPolyPoint in_thirdPoint, int in_lineID);
	void determineType2and3Lines();		// calculates the Type 2 and Type 3 lines for each line in the triangle
	void rotateTriangleFromZAxis(int in_Point);
	void determineLineAngles();			// determines two angles for each line: an angle of rotation towards y axis, and an angle towards the 3rd point of the triangle
	void insertTracedBlueprint(EnclaveKeyDef::EnclaveKey in_key);
	bool checkIfPointsAreInSameBlueprint();
	void addNewPrimarySegment(ECBPolyPoint in_lineSegmentPointA, ECBPolyPoint in_lineSegmentPointB, int in_lineID, EnclaveKeyDef::EnclaveKey in_blueprintKey);
	void fillMetaDataInPrimaryCircuits();
	void printPrimarySegmentData();
};

#endif