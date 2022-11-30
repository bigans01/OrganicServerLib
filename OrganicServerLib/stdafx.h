// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

// Some important notes on stdafx.h:
//
// -the order of includes DOES matter. (i.e, BoundaryOrientation.h must come before BoundaryPolyIndicator.h, because 
//	BoundaryPolyIndicator requires BoundaryOrientation)
//
// -given the previous point, we should always start with the standard C++ STL headers, followed by ascending order of 
//	dependency level -- i.e, level 0 is the OrganicIndependentsLib, level 1 is OrganicGLWinLib, OrganicPolyOperationsLib, OrganicWindowsAdapter
//	and level 2 is OrganicCoreLib, etc.

#pragma once

#include "targetver.h"

// Standard C++ headers
#include <glm/glm.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <stack>
#include <queue>

// -------------------------------Dependency Level 0 Headers
// Custom headers, from OrganicIndependents.lib
#include "ECBPolyPoint.h"
#include "DoublePoint.h"
#include "TriangleMaterial.h"
#include "ECBPolyPointPair.h"
#include "EnclaveKeyDef.h"
#include "BoundaryOrientation.h"
#include "BoundaryPolyIndicator.h"
#include "UniquePointContainer.h"
#include "Message.h"
#include "OperableIntSet.h"
#include "IndependentUtils.h"
#include "FRayCasterTypeEnum.h"

// -------------------------------Dependency Level 1 Headers
// Custom headers, from OrganicGLWinLib.lib

// -------------------------------Dependency Level 2 Headers
#include "OrganicSystem.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here
