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
#include "ServerJobRunVerdict.h"
#include "ServerThreadDesignationMap.h"

class OSServer;
class ServerPhasedJobBase
{
	public:
		virtual void interpretMessage(Message in_message) = 0;		// all phased jobs will be interpreting incoming messages differently;
																	// depending on how the ServerPhasedJobBase-derived class interprets this message, the class can do things like:
																	//	-insert new jobs into an existing phase for a run, therefore extending the life of the phase by one server tick per added job
																	//  -check the OrganicServer to see if it is time for an indefinitely-running job to quit (for instance, sounds or temporary world events)

		virtual void initializeAndSetOptionalSPJMetadata(Message in_message) = 0;								// initialize the SPJ, and be sure to set the value of requiredThreadDesignation for that class;
																	// additionally, the SPJ can optionally set metadata from a Message that is passed in.
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
		void setLocation(int in_spjLayerID, int in_spjLayerSmartID);
		void insertStringedMessage(std::string in_stringName, Message in_message);

		// parent key values
		std::string requiredThreadDesignation = "";	// must be set for all descendants of this base class, in that specific class.

		JobPhaseState getCurrentPhaseState();									// should be called every time we are checking the job, before anything else

		void removeCompletedPhaseSubJobs();		// For the current phase, this takes proper steps to remove any completed ServerJobBase-derived classes, 
												// and then flags the current phase as being complete. If it is last phase of the derived ServerPhasedJobBase class,
												// the jobState of this class wil lbe set to ServerJobState::COMPLETE.												

		void insertNewPhase(std::shared_ptr<ServerJobPhase> in_phasePtr);
		void insertNewPhases(int in_phaseCount);
		bool checkIfCurrentPhaseIsInProgress();
		Message buildCompletionMessageForSJ(int in_currentPhaseIndex, int in_currentJobMapKey);

		ReadyJobSearch findNextWaitingJob();
		bool isCurrentJobRunnable(ServerThreadDesignationMap* in_serverThreadDesignationMapRef);			// returns true if verdict is good (it can run)
		
		std::string fetchThreadDesignation();

		// job location
		int spjLayerID = 0;
		int spjLayerSmartID = 0;
};

#endif
