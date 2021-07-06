#pragma once

#ifndef ECBPOLYREFORMERTYPE_H
#define ECBPOLYREFORMERTYPE_H

enum class ECBPolyReformerType
{
	UNDEFINED,
	REFORM_BY_COLLISION,	// used when both the persistent existing mass contains ECBPolys, AND the ContourPlan also has shell ECBPolys
	REFORM_BY_OVERWRITE		// used when only the persistent existing mass was detected as having ECBPolys, but the ContourPlan did not;
							// this would mean that the ContourPlan overwrote the entire mass of the persistent mass, therefore erasing all 
							// of the previously persistent ECBPolys
};

#endif
