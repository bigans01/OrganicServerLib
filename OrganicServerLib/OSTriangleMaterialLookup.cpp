#include "stdafx.h"
#include "OSTriangleMaterialLookup.h"

std::set<TriangleMaterial> OSTriangleMaterialLookup::materialEnumToMaterialIDSet =
{
	{TriangleMaterial::DIRT},
	{TriangleMaterial::GRASS}
};

TriangleMaterial OSTriangleMaterialLookup::findMaterialID(TriangleMaterial in_materialEnum)
{
	TriangleMaterial returnID = TriangleMaterial::DIRT;	// if no value is found, return a value of dirt (1)
	auto materialFinder = materialEnumToMaterialIDSet.find(in_materialEnum);
	if (materialFinder != materialEnumToMaterialIDSet.end())
	{
		returnID = *materialFinder;
	}
	return returnID;
}