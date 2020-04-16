#include "stdafx.h"
#include "ForgedPolyRegistry.h"

void ForgedPolyRegistry::addToPolyset(EnclaveKeyDef::EnclaveKey in_blueprintKey, int in_polyID)
{
	polySetRegistry[in_blueprintKey].polySet.insert(in_polyID);
}