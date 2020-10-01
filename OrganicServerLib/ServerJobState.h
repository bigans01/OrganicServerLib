#pragma once

#ifndef SERVERJOBSTATE_H
#define SERVERJOBSTATE_H

enum class ServerJobState
{
	WAITING_TO_EXECUTE,
	RUNNING,
	COMPLETE,
	FAILED
};

#endif
