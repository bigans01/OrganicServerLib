#pragma once

#ifndef SERVERJOBSTATE_H
#define SERVERJOBSTATE_H

/*

Description: Simple enum class used in both ServerJobBase and ServerPhasedJobBase-derived classes,
that can be used to easily indicate the status of a job, in a human-readable manner.

*/

enum class ServerJobState
{
	WAITING_TO_EXECUTE,
	RUNNING,
	COMPLETE,
	FAILED
};

#endif
