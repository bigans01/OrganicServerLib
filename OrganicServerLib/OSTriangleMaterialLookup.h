#pragma once

#ifndef OSTRIANGLEMATERIALLOOKUP_H
#define OSTRIANGLEMATERIALLOOKUP_H

#include <map>
#include "OSTriangleMaterial.h"

class OSTriangleMaterialLookup
{
	public:
		static std::set<TriangleMaterial> materialEnumToMaterialIDSet;
		static TriangleMaterial findMaterialID(TriangleMaterial in_materialEnum);
};

#endif
