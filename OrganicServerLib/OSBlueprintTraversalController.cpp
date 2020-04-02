#include "stdafx.h"
#include "OSBlueprintTraversalController.h"

OSBlueprintTraversalController::OSBlueprintTraversalController(PrimaryLineT1 in_primaryLine, int in_lineID)
{
	lineMeta = in_primaryLine;
	lineID = in_lineID;
}

bool OSBlueprintTraversalController::isLineContainedToOneBlueprint()
{
	bool result = false;		// default is false (they are not in same blueprint)
	if
	(
		(lineMeta.beginPointBlueprintKey.x == lineMeta.endPointBlueprintKey.x)
		&&
		(lineMeta.beginPointBlueprintKey.y == lineMeta.endPointBlueprintKey.y)
		&&
		(lineMeta.beginPointBlueprintKey.z == lineMeta.endPointBlueprintKey.z)
	)
	{
		//std::cout << "!! Points of line ARE in same blueprint! " << std::endl;
		result = true;
	}
	else
	{
		//std::cout << "!! Points of line ARE NOT in same blueprint! " << std::endl;
	}
	return result;
}