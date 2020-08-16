#pragma once

#ifndef OSSERVERUTILS_H
#define OSSERVERUTILS_H

#include "ECBPoly.h"
#include "OSContouredTriangle.h"
#include "EnclaveCollectionBlueprint.h"
#include "ECBIntersectMeta.h"
#include "BlueprintTransformRefs.h"
#include "OSWinAdapter.h"

class OSServerUtils
{
	public:
		static void fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle);
		static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID);
		static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint);
		static void analyzePolyValidityAndInsert(
												OSContouredTriangle* in_contouredTriangle, 
												ECBPolyPoint in_segmentPointA,
												ECBPolyPoint in_segmentPointB,
												int in_lineID,
												EnclaveKeyDef::EnclaveKey in_currentTraceKey,
												BorderDataMap* in_dataMapRef, 
												EnclaveCollectionBlueprint* in_blueprintPtr, 
												ECBPoly* in_polyToInsertRef);
		static void writeBlueprintToDisk(std::string in_worldName, 
										EnclaveKeyDef::EnclaveKey in_blueprintKey, 
										std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef);

};

#endif
