#pragma once

#ifndef SERVERJOBPHASE_H
#define SERVERJOBPHASE_H

#include <map>
#include <mutex>
#include "ServerJobBase.h"
#include "JobPhaseState.h"


class ServerJobPhase
{
	public:

		JobPhaseState phaseState = JobPhaseState::IDLE;
		std::map<int, std::shared_ptr<ServerJobBase>> jobMap;
		std::map<int, std::shared_ptr<ServerJobBase>>::iterator currentJobIter;
		std::vector<int> completedVector;
		//std::vector<int> completedVector;												// a vector that contains a list of completed elements.
		int currentJobsCompleted = 0;												// the total jobs in this phase that have been completed so far.
		int requiredJobsToBeCompleted = 0;											// the total number of required jobs that must be completed before this phase is considered as "done"

		JobPhaseState calculateAndReturnPhaseState();

		// job removal
		void removeDonePhaseSubJobs();

		void loadDonePhaseSubjobs();
		void eraseDonePhaseSubJobs();
};

#endif
