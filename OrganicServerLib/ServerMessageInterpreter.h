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

		// message handling functions; each function below should have logic for handling LOCAL and REMOTE message types.
		void handleRequestFromClientForOGLMRMCBlueprints(Message in_message);
		void handleRequestFromClientGetBlueprintForT1(Message in_message);
		void handleRequestFromClientGetBlueprintForT2(Message in_message);
		void handleRequestFromClientRunContourPlan(Message in_message);
		void handleRequestToClientSetWorldDirection(Message in_message);
};

#endif
