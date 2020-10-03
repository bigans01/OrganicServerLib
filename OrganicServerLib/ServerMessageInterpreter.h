#pragma once

#ifndef SERVERMESSAGEINTERPRETER_H
#define SERVERMESSAGEINTERPRETER_H

#include "MessageCable.h"
#include "MessageLocality.h"
#include "MessageInterpreter.h"
#include <iostream>
#include "EnclaveKeyDef.h"
#include "BlueprintScanningCuboid.h"

class OSServer;	
class ServerMessageInterpreter : public MessageInterpreter
{
	public:
		OSServer* serverPtr = NULL;
		MessageCable* messageCableRef = NULL;
		void initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef);

		// client request and response
		void interpretIncomingMessagesFromClient();
		void interpretOutgoingMessagesToClient();
};

#endif
