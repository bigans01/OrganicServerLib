#pragma once

#ifndef OSTRIANGLEMATERIALLOOKUP_H
#define OSTRIANGLEMATERIALLOOKUP_H

#include <map>
#include "OSTriangleMaterial.h"

class OSTriangleMaterialLookup
{
	public:
		static std::map<OSTriangleMaterial, int> materialEnumToMaterialIDMap;
		static int findMaterialID(OSTriangleMaterial in_materialEnum);
};

#endif
