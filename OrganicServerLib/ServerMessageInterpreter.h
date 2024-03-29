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
	private:
		friend class OSServer;

		OSServer* serverPtr = NULL;
		MessageCable* messageCableRef = NULL;
		void initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef);

		// client request and response
		void interpretIncomingMessagesFromClient();
		void interpretOutgoingMessagesToClient();

		// message handling functions; each function below should have logic for handling LOCAL and REMOTE message types.
		void handleRequestFromClientEnableORECompositionHighlighting(Message in_message);
		void handleRequestFromClientDisableORECompositionHighlighting(Message in_message);
		void handleRequestFromClientGetBlockData(Message in_message);
		void handleRequestFromClientForOGLMRMCBlueprints(Message in_message);
		void handleRequestFromClientGetBlueprintForT1(Message in_message);
		void handleRequestFromClientGetBlueprintForT2(Message in_message);
		void handleRequestFromClientRunContourPlan(Message in_message);
		void handleRequestToClientSetWorldDirection(Message in_message);
		void handleRequestToClientSendCurrentClientOGLMRMC(Message in_message);
		void handleRequestFromClientToggleBlockHighlighting(Message in_message);				// MESSAGE CHAIN: toggleBlockHighlighting
		void handleRequestFromClientToggleCurrentEnclaveHighlighting(Message in_message);		// MESSAGE CHAIN: toggleCurrentEnclaveHighlighting
		void handleRequestFromClientOGLMToggleWorldLocation(Message in_message);				// MESSAGE CHAIN: toggleWorldLocationPanel
		void handleRequestFromClientInputGenerated(Message in_message);
		void handleRequestFromClientMouseWheelScroll(Message in_message);
		void handleRequestFromClientBlockDelete(Message in_message);							// MESSAGE CHAIN: clientRequestsBlockDelete 
};

#endif
