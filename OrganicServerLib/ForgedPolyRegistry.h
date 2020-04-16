#pragma once

#ifndef FORGEDPOLYREGISTRY_H
#define FORGEDPOLYREGISTRY_H

#include "EnclaveKeyDef.h"
#include <unordered_map>
#include "ForgedPolySet.h"


class ForgedPolyRegistry
{
public: 
	std::unordered_map<EnclaveKeyDef::EnclaveKey, ForgedPolySet, EnclaveKeyDef::KeyHasher> polySetRegistry;
	void addToPolyset(EnclaveKeyDef::EnclaveKey in_blueprintKey, int in_polyID);
};

#endif
