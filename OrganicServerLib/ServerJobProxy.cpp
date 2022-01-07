#include "stdafx.h"
#include "ServerJobProxy.h"
#include "OSServer.h"

void ServerJobProxy::callServerJobRunSingleMountTest(OSServer* in_serverRef, Message in_mountainMetaData)
{
	in_serverRef->constructSingleMountTestNoInput(in_mountainMetaData);
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

bool ServerJobProxy::checkIfServerJobBlockingFlagExists(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck)
{
	return in_serverRef->serverJobManager.doesFlagExist(in_flagToCheck);
}