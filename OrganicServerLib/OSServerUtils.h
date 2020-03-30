#pragma once

#ifndef OSSERVERUTILS_H
#define OSSERVERUTILS_H

#include "ECBPoly.h"
#include "OSContouredTriangle.h"

class OSServerUtils
{
	public:
		static void fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle);
		static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID);
		static void fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint);
};

#endif
