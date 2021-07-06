#pragma once

#ifndef ECBPOLYREFORMER_H
#define ECBPOLYREFORMER_H

#include "OrganicTriangleTracker.h"
#include "OperableIntSet.h"
#include "ECBPolyReformerType.h"
#include "EnclaveFractureResultsMap.h"
#include <map>

class ECBPolyReformer
{
	public:
		ECBPolyReformer() {};
		ECBPolyReformer(ECBPolyReformerType in_reformerType,
			OperableIntSet in_contouredMassShellECBPolyIDs,
			EnclaveFractureResultsMap* in_contouredMassContentsRef,
			OperableIntSet in_persistentMassShellECBPolyIDs,
			EnclaveFractureResultsMap* in_persistentMassContentsRef) :
		reformerType(in_reformerType),
		contouredMassShellECBPolyIDs(in_contouredMassShellECBPolyIDs),
		contouredMassContentsRef(in_contouredMassContentsRef),
		persistentMassShellECBPolyIDs(in_persistentMassShellECBPolyIDs),
		persistentMassContentsRef(in_persistentMassContentsRef)
		{};

		void produceFirstPassECBPolys(int* in_nextECBPolyIDTrackerRef);
					

		ECBPolyReformerType reformerType = ECBPolyReformerType::UNDEFINED;	// will be set by constructor
		OperableIntSet contouredMassShellECBPolyIDs;
		EnclaveFractureResultsMap* contouredMassContentsRef = nullptr;
		OperableIntSet persistentMassShellECBPolyIDs;
		EnclaveFractureResultsMap* persistentMassContentsRef = nullptr;

		OrganicTriangleTracker reformerTracker;

	private:
		std::map<int, std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>> firstPassOutPutIDs;
};

#endif
