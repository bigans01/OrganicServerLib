#pragma once

#ifndef SERVERJOBBLOCKINGFLAGS_H
#define SERVERJOBBLOCKINGFLAGS_H

enum class ServerJobBlockingFlags
{
	SERVER_RUNNING_CONTOUR_PLAN,	// indicates that the server is currently running a contour plan; only one contour plan may be run at a time. 
									// a Contoured Plan that needs to run must acquire this flag before running.
	SERVER_BUILDING_AFFECTED_BLUEPRINTS_FOR_PLAN	// indicates that the current running contour plan is attempting to estimate it's affected blueprints.
													// No blueprint modification can be done while this is running
};

#endif
