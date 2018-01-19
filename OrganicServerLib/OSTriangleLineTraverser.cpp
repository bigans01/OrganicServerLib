#include "stdafx.h"
#include <iostream>
#include "OSTriangleLineTraverser.h"

OSTriangleLineTraverser::OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID)
{
	if (in_lineID < 2)
	{
		beginKey = in_TrianglePtr->pointKeys[in_lineID];		// set begin key
		currentKey = in_TrianglePtr->pointKeys[in_lineID];		// current key is same as begin key
		endKey = in_TrianglePtr->pointKeys[in_lineID + 1];		// set end key

		beginPoint = in_TrianglePtr->triangleLines[in_lineID].pointA;	// set begin and end points
		endPoint = in_TrianglePtr->triangleLines[in_lineID].pointB;
	}
	else if (in_lineID == 2)
	{
		beginKey = in_TrianglePtr->pointKeys[2];
		currentKey = in_TrianglePtr->pointKeys[2];		// current key is same as begin key
		endKey = in_TrianglePtr->pointKeys[0];

		beginPoint = in_TrianglePtr->triangleLines[2].pointA;	// set begin and end points
		endPoint = in_TrianglePtr->triangleLines[2].pointB;
	}

	//ECBBorderLineList borderLineList;
	//borderLineList = OrganicUtils::determineBorderLines(beginKey);		// get the border line list for the first point
	ECBFaceIntersect resultantIntersect = OrganicUtils::findClosestIntersection(beginPoint, endPoint, beginKey, endKey);

}

