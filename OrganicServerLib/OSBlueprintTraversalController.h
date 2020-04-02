#pragma once

#ifndef OSBLUEPRINTTRAVERSALCONTROLLER_H
#define OSBLUEPRINTTRAVERSALCONTROLLER_H

#include "PrimaryLineT1.h"
#include "OSTriangleLineTraverserBare.h"

class OSBlueprintTraversalController
{
	public:
		OSBlueprintTraversalController(PrimaryLineT1 in_primaryLine, int in_lineID);
		PrimaryLineT1 lineMeta;								// primary line data
		OSTriangleLineTraverserBare blueprintTraverser;		// the blueprint traverser
		int lineID = 0;				// ID of the line; from the lineMeta

		bool isLineContainedToOneBlueprint();	// checks to see if the line is contained to a single blueprint; no traversal will occur if it is contained to a single blueprint.

};

#endif
