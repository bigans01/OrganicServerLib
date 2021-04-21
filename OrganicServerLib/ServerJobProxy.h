#pragma once

#ifndef SERVERJOBPROXY_H
#define SERVERJOBPROXY_H

#include "Message.h"
#include <iostream>

class OSServer;
class ServerJobProxy
{
	public:
		static void callServerJobRunSingleMountTest(OSServer* in_serverRef);
		static void callServerJobRunBigMountTest(OSServer* in_serverRef);
		static void callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message);
		static void callServerSendOutgoingMessageToMessageInterpreter(OSServer* in_serverRef, Message in_message);
};

#endif
