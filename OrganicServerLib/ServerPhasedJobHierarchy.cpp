#include "stdafx.h"
#include "ServerPhasedJobHierarchy.h"
#include "OSServer.h"

void ServerPhasedJobHierarchy::setServerPtr(OSServer* in_server)
{
	server = in_server;
}

void ServerPhasedJobHierarchy::insertSPJ(int in_layerToInsertTo, std::shared_ptr<ServerPhasedJobBase>* in_jobRef, Message in_initialMessage)	
{
	std::cout << "!! Insert job into int container, at index: " << in_layerToInsertTo << std::endl;
	int targetLayerJobID = hierarchy[in_layerToInsertTo].getNextAvailableKey();		// Get the unique ID of the job, that it will need for when it is inserted into the layer
	(hierarchy[in_layerToInsertTo])[targetLayerJobID] = *in_jobRef;					// Next, insert the job before anything else
	(hierarchy[in_layerToInsertTo])[targetLayerJobID]->initializeBaseClass(server, in_initialMessage.messageLocality, ServerJobContainerType::INT);
	(hierarchy[in_layerToInsertTo])[targetLayerJobID]->setLocation(in_layerToInsertTo, targetLayerJobID);	// ensure that the SPJ knows where it's at (needed for completion messages)
	(hierarchy[in_layerToInsertTo])[targetLayerJobID]->initializeAndSetOptionalSPJMetadata(std::move(in_initialMessage));	// initialize the derived class that inherits the base class; 
																															// set optional metadata if need be by moving it into the SPJ.
}

void ServerPhasedJobHierarchy::cleanupJobsInPhasedJobs()	
{
	// Check for any ServerJobBase instances, in all ServerPhasedJobBase-based jobs,
	// to see if any have completed; for the ones that have completed, run post-processing tasks 
	// (i.e, through ServerJobBase::runPostCompleteTasks()) before removing the jobs.
	auto currentLayerBegin = hierarchy.begin();
	auto currentLayerEnd = hierarchy.end();

	// Cycle through each layer
	for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
	{
		// Go through each ServerPhasedJob in the current layer
		auto currentPhasedJobBegin = currentLayerBegin->second.begin();
		auto currentPhasedJobEnd = currentLayerBegin->second.end();
		for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
		{
			// run post-processing and removal of any completed ServerJobBase-derived jobs, in the current phase 
			// we are looking at (if there are any to do, that is)
			currentPhasedJobBegin->second->removeCompletedPhaseSubJobs();
		}
	}
}

void ServerPhasedJobHierarchy::removeCompletedPhasedJobs()	
{
	// This function will remove any SPJ that has been flagged as COMPLETE.
	auto currentLayerBegin = hierarchy.begin();
	auto currentLayerEnd = hierarchy.end();
	for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
	{
		auto currentPhasedJobBegin = currentLayerBegin->second.begin();
		auto currentPhasedJobEnd = currentLayerBegin->second.end();
		for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
		{
			if (currentPhasedJobBegin->second->jobState == ServerJobState::COMPLETE)
			{
				// insert into the removal queue, if detected as COMPLETE.
				std::cout << "!! Job to remove found! " << std::endl;
				HierarchyLocation newLocation(currentLayerBegin->first, currentPhasedJobBegin->first);
				removalQueue.push(newLocation);
			}
		}
	}
	processRemovalQueue();
}

void ServerPhasedJobHierarchy::handleUpdateMessage(Message in_updateMessage)
{
	// Reads an update message's layer ID and unique ID, to determine where to send it; it should then
	// "cut" the layer ID and unique ID as they are no longer needed (test this, see how it fits)
	in_updateMessage.open();
	int messageLayerID = in_updateMessage.readInt();
	int messageLayerUniqueID = in_updateMessage.readInt();
	(hierarchy[messageLayerID])[messageLayerUniqueID]->interpretMessage(std::move(in_updateMessage));
}

std::vector<std::shared_ptr<ServerPhasedJobBase>> ServerPhasedJobHierarchy::produceRunnableJobs()
{
	// Returns a vector of shared_ptrs, that the ServerJobManager class will use to attempt to submit each job.
	// This makes the code in ServerJobManager cleaner, and leaves the determination of what jobs can run to be done by ServerPhasedJobHierarchy.
	// This class will also be able to control what jobs get inserted into the return vector, by some yet-to-be-defined criteria.
	std::vector<std::shared_ptr<ServerPhasedJobBase>> returnJobs;
	auto currentLayerBegin = hierarchy.begin();
	auto currentLayerEnd = hierarchy.end();
	for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
	{
		auto currentPhasedJobBegin = currentLayerBegin->second.begin();
		auto currentPhasedJobEnd = currentLayerBegin->second.end();
		for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
		{
			returnJobs.push_back(currentPhasedJobBegin->second);
		}
	}
	return returnJobs;
}

int ServerPhasedJobHierarchy::numberOfJobsInHierarchy()	
{
	// Returns the overall number of jobs in the hierarchy.
	int jobCount = 0;
	auto currentLayerBegin = hierarchy.begin();
	auto currentLayerEnd = hierarchy.end();
	for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
	{
		auto currentPhasedJobBegin = currentLayerBegin->second.begin();
		auto currentPhasedJobEnd = currentLayerBegin->second.end();
		for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
		{
			jobCount++;
		}
	}
	return jobCount;
}

void ServerPhasedJobHierarchy::processRemovalQueue()
{
	while (!removalQueue.empty())
	{
		auto currentLocation = removalQueue.front();
		hierarchy[currentLocation.layerID].erase(currentLocation.layerMapID);
		std::cout << "Removing job at layer " << currentLocation.layerID << ", unique ID " << currentLocation.layerMapID << std::endl;
		removalQueue.pop();
	}
}