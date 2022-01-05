#pragma once

#ifndef SERVERJOBBLOCKINGFLAGS_H
#define SERVERJOBBLOCKINGFLAGS_H

enum class ServerJobBlockingFlags
{
	SERVER_RUNNING_CONTOUR_PLAN,	// indicates that the server is currently running a contour plan; only one contour plan may be run at a time. 
									// a Contoured Plan that needs to run must acquire this flag before running.
	HALT_FUTURE_COLLECTION_MODIFICATIONS,	// used to signal that any collection modification jobs that are waiting to be submitted must wait until this flag isn't up.
	SERVER_BUILDING_AFFECTED_BLUEPRINTS_FOR_PLAN	// indicates that the current running contour plan is attempting to estimate it's affected blueprints.
													// No blueprint modification can be done while this is running
};

#endif
