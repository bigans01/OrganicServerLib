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
		static void callServerJobRunSingleMountTest(OSServer* in_serverRef);
		static void callServerJobRunBigMountTest(OSServer* in_serverRef);
		static void callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message);
		static void callServerSendOutgoingMessageToMessageInterpreter(OSServer* in_serverRef, Message in_message);
		static bool checkIfServerJobBlockingFlagExists(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
};

#endif
