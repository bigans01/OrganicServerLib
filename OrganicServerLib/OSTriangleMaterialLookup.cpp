#include "stdafx.h"
#include "OSTriangleMaterialLookup.h"

std::map<OSTriangleMaterial, int> OSTriangleMaterialLookup::materialEnumToMaterialIDMap =
{
	{OSTriangleMaterial::DIRT, 1},
	{OSTriangleMaterial::GRASS, 2}
};

int OSTriangleMaterialLookup::findMaterialID(OSTriangleMaterial in_materialEnum)
{
	int returnID = 1;	// if no value is found, return a value of dirt (1)
	auto materialFinder = materialEnumToMaterialIDMap.find(in_materialEnum);
	if (materialFinder != materialEnumToMaterialIDMap.end())
	{
		returnID = materialFinder->second;
	}
	return returnID;
}