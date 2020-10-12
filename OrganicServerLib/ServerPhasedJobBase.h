#pragma once

#ifndef SERVERPHASEDJOBBASE_H
#define SERVERPHASEDJOBBASE_H

#include "Message.h"
#include "MessageLocality.h"
#include "ServerJobContainerType.h"
#include "ServerJobPhase.h"
#include "ServerJobState.h"
#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include "ReadyJobSearch.h"

class OSServer;
class ServerPhasedJobBase
{
	public:
		virtual void interpretMessage(Message in_message) = 0;		// all phased jobs will be interpreting incoming messages diffrently
		virtual void initialize() = 0;								// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		virtual void initializeCurrentPhase() = 0;					// would be called after initialize()
		int currentPhaseIndex = 0;									

		std::map<int, std::shared_ptr<ServerJobPhase>> phaseMap;
		std::map<int, std::shared_ptr<ServerJobPhase>>::iterator currentPhaseIter;
		std::unordered_map<std::string, Message> stringedMessageMap;

		OSServer* server;
		ServerJobContainerType parentContainerType = ServerJobContainerType::NOVAL;
		MessageLocality locality = MessageLocality::LOCAL;		// default is LOCAL, but may be changed to REMOTE at a later time.
		ServerJobState jobState = ServerJobState::RUNNING;		// can probably be something other than "running"

		void initializeBaseClass(OSServer* in_serverPtr, MessageLocality in_locality, ServerJobContainerType in_parentContainerType);
		void insertParentContainerIntKey(int in_parentContainerIntKey);
		void insertParentContainerStringKey(std::string in_parentContainerStringKey);
		void insertStringedMessage(std::string in_stringName, Message in_message);

		// parent key values
		int parentContainerIntKey = 0;
		std::string parentContainerStringKey = "";

		JobPhaseState getCurrentPhaseState();									// should be called every time we are checking the job, before anything else
		void removeCompletedPhaseSubJobs();
		void insertNewPhase(std::shared_ptr<ServerJobPhase> in_phasePtr);
		bool checkIfCurrentPhaseIsInProgress();

		ReadyJobSearch findNextWaitingJob();

		// job removal
		//void removeDonePhaseSubJobs();

		//void loadDonePhaseSubjobs();
		//void eraseDonePhaseSubJobs();
};

#endif
