#pragma once

#ifndef OSTRIANGLELINETRAVERSER_H
#define OSTRIANGLELINETRAVERSER_H

#include "OSContouredTriangle.h"
#include "EnclaveKeyDef.h"
#include "OrganicUtils.h"
#include "ECBFaceIntersect.h"
#include "EnclaveCollectionBlueprint.h"
#include "OSServerUtils.h"
#include "BorderDataMap.h"

class OSTriangleLineTraverser
{
public:
	//OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID, OSServer* in_serverPtr);
	OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapPtr);
	EnclaveKeyDef::EnclaveKey beginKey;		// the beginning key = the key that point A lies in
	EnclaveKeyDef::EnclaveKey currentKey;	// the current key value
	EnclaveKeyDef::EnclaveKey nextKeyAdd;	// how much will be added to currentKey in the next iteration
	EnclaveKeyDef::EnclaveKey endKey;		// the ending key = the key that point B lies in (will be done when this happens)
	OSContouredTriangle* contouredTriangleRef;
	BorderDataMap borderData;

	ECBPolyPoint beginPoint;				// equals point A of line
	ECBPolyPoint currentIterationEndpoint;	// equals whatever the point is when this line hits an ECB border
	ECBPolyPoint endPoint;					// equals point B of line
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* blueprintMapRef = NULL;

	int lineID = 0;							// ID of the line, in relation to the triangle it belongs to (0, 1, or 2)
	void traverseLineOnce(OSContouredTriangle* in_TrianglePtr);				// traverse the line through one blueprint (used if both line points are not in same blueprint)
};

#endif
