#pragma once

#ifndef CONTOURPLANSTATE_H
#define CONTOURPLANSTATE_H

enum class ContourPlanState
{
	WAITING_TO_RUN,
	RUNNING,
	COMPLETE,
	FAILED
};

#endif
