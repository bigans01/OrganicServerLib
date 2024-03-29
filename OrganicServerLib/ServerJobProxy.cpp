#include "stdafx.h"
#include "ServerJobProxy.h"
#include "OSServer.h"

void ServerJobProxy::callServerJobPrepCPMountain(OSServer* in_serverRef, Message in_mountainMetaData)
{
	in_serverRef->prepCPMountain(in_mountainMetaData);
}

void ServerJobProxy::callServerRunContourPlanWorldTracing(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->runContourPlanWorldTracing(in_contourPlanName);
}

void ServerJobProxy::callServerBuildContourPlanAffectedBlueprints(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->buildContourPlanAffectedBlueprints(in_contourPlanName);
}

void ServerJobProxy::callServerRunContourPlanFracturingAndMassDriving(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->runContourPlanFracturingAndMassDriving(in_contourPlanName);
}

void ServerJobProxy::callServerRunCPV2(OSServer* in_serverRef, Message in_cpv2Metadata)
{
	in_serverRef->runCPV2SPJ(in_cpv2Metadata);
}

void ServerJobProxy::callServerAddAndBuildCPV2MeshForSPJ(OSServer* in_serverRef, Message in_cpv2Metadata)
{
	in_serverRef->addAndBuildCPV2MeshForSPJ(in_cpv2Metadata);
}

bool ServerJobProxy::callServerAttemptCPV2KeyLoadIntoHotKeys(OSServer* in_serverRef)
{
	return in_serverRef->attemptCPV2KeyLoadIntoHotKeys();
}

void ServerJobProxy::callServerRunMassDriversForCPV2(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->runMassDriversForCPV2(in_contourPlanName);
}

void ServerJobProxy::callServerCleanupCPV2(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->cleanupCPV2(in_contourPlanName);
}

void ServerJobProxy::callServerJobRunBigMountTest(OSServer* in_serverRef)
{
	in_serverRef->constructBigMountTestNoInput();
}

void ServerJobProxy::callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message)
{
	std::cout << "!!! calling send update... " << std::endl;
	in_serverRef->serverJobManager.updateMessages.insertUpdate(in_message);
	//std::cout << "######## update inserted successfully. " << std::endl;
}

void ServerJobProxy::callServerSendOutgoingMessageToMessageInterpreter(OSServer* in_serverRef, Message in_message)
{
	in_serverRef->jobSendOutgoingMessageToInterpreter(std::move(in_message));
}

void ServerJobProxy::callServerGenerateBlueprintBackupsForCP(OSServer* in_serverRef, std::string in_planName)
{
	in_serverRef->generateBlueprintBackups(in_planName);
}

void ServerJobProxy::callServerGenerateBlueprintBackupsForCPV2SPJ(OSServer* in_serverRef)
{
	in_serverRef->generateBlueprintBackupsForCPV2SPJ();
}

void ServerJobProxy::callServerCopyOverCPV2ECBPolys(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->copyOverCPV2ECBPolys(in_contourPlanName);
}

void ServerJobProxy::callServerCheckContourPlanSuccess(OSServer* in_serverRef, std::string in_contourPlanName)
{
	in_serverRef->checkContourPlanSuccess(in_contourPlanName);
}

bool ServerJobProxy::checkIfServerJobBlockingFlagExists(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck)
{
	return in_serverRef->serverJobManager.doesFlagExist(in_flagToCheck);
}

void ServerJobProxy::activateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck)
{
	in_serverRef->serverJobManager.activateBlockingFlag(in_flagToCheck);
}

void ServerJobProxy::deactivateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck)
{
	in_serverRef->serverJobManager.deactivateBlockingFlag(in_flagToCheck);
}

bool ServerJobProxy::isJobCategoryAtZero(OSServer* in_serverRef, ServerJobRunCategory in_categoryToCheck)
{
	return in_serverRef->serverJobManager.runCounters.isCategoryAllZero(in_categoryToCheck);
}

bool ServerJobProxy::callAttemptFlagActivation(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck)
{
	return in_serverRef->serverJobManager.getAttemptedFlagRaiseResult(in_flagToCheck);
}

void ServerJobProxy::callDetermineAffectedBlueprintsForCPV2(OSServer* in_serverRef, std::string in_cpv2NameToDetermineFor)
{
	in_serverRef->determineAffectedBlueprintsForCPV2(in_cpv2NameToDetermineFor);
}