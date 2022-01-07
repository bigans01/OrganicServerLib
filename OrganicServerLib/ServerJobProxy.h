#pragma once

#ifndef SERVERJOBPROXY_H
#define SERVERJOBPROXY_H

#include "Message.h"
#include "ServerJobBlockingFlags.h"
#include <iostream>

class OSServer;
class ServerJobProxy
{
	public:
		static void callServerJobPrepCPMountain(OSServer* in_serverRef, Message in_mountainMetaData);
		static void callServerRunContourPlanWorldTracing(OSServer* in_serverRef, std::string in_contourPlanName);
		static void callServerBuildContourPlanAffectedBlueprints(OSServer* in_serverRef, std::string in_contourPlanName);
		static void callServerRunContourPlanFracturingAndMassDriving(OSServer* in_serverRef, std::string in_contourPlanName);
		static void callServerJobRunBigMountTest(OSServer* in_serverRef);
		static void callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message);
		static void callServerSendOutgoingMessageToMessageInterpreter(OSServer* in_serverRef, Message in_message);
		static bool checkIfServerJobBlockingFlagExists(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
		static void activateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
		static void deactivateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
};

#endif
