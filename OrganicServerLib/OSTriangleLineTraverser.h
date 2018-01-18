#pragma once

#ifndef OSTRIANGLELINETRAVERSER_H
#define OSTRIANGLELINETRAVERSER_H

#include "OSContouredTriangle.h"
#include "EnclaveKeyDef.h"
#include "OrganicUtils.h"

class OSTriangleLineTraverser
{
public:
	OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID);
	EnclaveKeyDef::EnclaveKey beginKey;		// the beginning key = the key that point A lies in
	EnclaveKeyDef::EnclaveKey currentKey;	// the current key value
	EnclaveKeyDef::EnclaveKey endKey;		// the ending key = the key that point B lies in (will be done when this happens)

	ECBPolyPoint beginPoint;				// equals point A of line
	ECBPolyPoint currentIterationEndpoint;	// equals whatever the point is when this line hits an ECB border
	ECBPolyPoint endPoint;					// equals point B of line
};

#endif
