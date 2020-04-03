#pragma once

#ifndef OSTRIANGLELINETRAVERSERBARE_H
#define OSTRIANGLELINETRAVERSERBARE_H

#include "OSContouredTriangle.h"
#include "PrimaryLineT1.h"
#include "OrganicUtils.h"
#include "ECBIntersectMeta.h"

class OSTriangleLineTraverserBare
{
	public:

		EnclaveKeyDef::EnclaveKey beginKey;		// the beginning key = the key that point A lies in
		EnclaveKeyDef::EnclaveKey currentKey;	// the current key value
		EnclaveKeyDef::EnclaveKey nextKeyAdd;	// how much will be added to currentKey in the next iteration
		EnclaveKeyDef::EnclaveKey endKey;		// the ending key = the key that point B lies in (will be done when this happens)

		ECBPolyPoint beginPoint;					// equals point A of line
		ECBPolyPoint currentIterationBeginPoint;	// equals whatever the begin point of the line is
		ECBPolyPoint currentIterationEndpoint;		// equals whatever the point is when this line hits an ECB border
		ECBPolyPoint endPoint;						//	 equals point B of line

		int isRunSingleIteration = 0;	// may not be needed
		short lineID = 0;				// may not be needed
		short isRunComplete = 0;
		
		void initialize(PrimaryLineT1* in_lineRef);
		bool checkIfRunComplete();			// check if the run is complete
		void traverseLineOnce();

};

#endif