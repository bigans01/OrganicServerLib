#include "stdafx.h"
#include "ServerMessageInterpreter.h"
#include "OSServer.h"

void ServerMessageInterpreter::initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef)
{
	serverPtr = in_organicSystemServerRef;
	messageCableRef = in_messageCableRef;
}

void ServerMessageInterpreter::interpretIncomingMessagesFromClient()	// for interpreting inbound requests from a client
{
	while (!messageCableRef->isIncomingQueueEmpty())
	{
		Message* currentMessage = messageCableRef->getIncomingMessageRefFromFront();
		currentMessage->open();					// open the message for reading (sets the iterators)
		switch (currentMessage->messageType)											
		{
			case MessageType::REQUEST_FROM_CLIENT_BLUEPRINTS_FOR_OGLMBUFFERMANAGER :	{ handleRequestFromClientForOGLMRMCBlueprints(std::move(*currentMessage));  break;  }
			case MessageType::REQUEST_FROM_CLIENT_GET_BLUEPRINT_FOR_T1 :				{ handleRequestFromClientGetBlueprintForT1(std::move(*currentMessage));  break;  }
			case MessageType::REQUEST_FROM_CLIENT_GET_BLUEPRINT_FOR_T2 :				{ handleRequestFromClientGetBlueprintForT2(std::move(*currentMessage));  break;  }
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN :					{ handleRequestFromClientRunContourPlan(std::move(*currentMessage));  break;  }
			case MessageType::REQUEST_FROM_CLIENT_TOGGLE_BLOCK_HIGHLIGHTING:			{ handleRequestFromClientToggleBlockHighlighting(std::move(*currentMessage)); break; }
			case MessageType::REQUEST_FROM_CLIENT_TOGGLE_CURRENT_ENCLAVE_HIGHLIGHTING:	{ handleRequestFromClientToggleCurrentEnclaveHighlighting(std::move(*currentMessage)); break; }
			case MessageType::REQUEST_FROM_CLIENT_TOGGLE_IMGUI_WORLD_LOCATION:			{ handleRequestFromClientOGLMToggleWorldLocation(std::move(*currentMessage)); break;	};
			case MessageType::REQUEST_FROM_CLIENT_INPUT_GENERATED:						{ handleRequestFromClientInputGenerated(std::move(*currentMessage)); break; };
			case MessageType::REQUEST_FROM_CLIENT_MOUSE_SCROLL_INPUT_GENERATED:			{ handleRequestFromClientMouseWheelScroll(std::move(*currentMessage)); break;}
			case MessageType::REQUEST_FROM_CLIENT_ENABLE_ORE_COMPOSITION_HIGHLIGHTING:	{ handleRequestFromClientEnableORECompositionHighlighting(std::move(*currentMessage)); break;}
			case MessageType::REQUEST_FROM_CLIENT_DISABLE_ORE_COMPOSITION_HIGHLIGHTING: { handleRequestFromClientDisableORECompositionHighlighting(std::move(*currentMessage)); break;}
			case MessageType::REQUEST_FROM_CLIENT_REMOVE_BLOCK:							{ handleRequestFromClientBlockDelete(std::move(*currentMessage)); break;}
			case MessageType::REQUEST_FROM_CLIENT_GET_BLOCK_DATA:						{ handleRequestFromClientGetBlockData(std::move(*currentMessage)); break;}
		}
		messageCableRef->popIncomingQueue();
	}

	while (!messageCableRef->isIncomingContainerQueueEmpty())
	{
		MessageContainer* currentContainer = messageCableRef->getIncomingMessageContainerRefFromFront();

		messageCableRef->popIncomingContainerQueue();
	}
}

void ServerMessageInterpreter::interpretOutgoingMessagesToClient()
{
	while (!messageCableRef->isOutgoingQueueEmpty())
	{
		std::cout << "(ServerMessageInterpreter): ############### Interpreting outbound messages... " << std::endl;
		Message* currentMessage = messageCableRef->getOutgoingMessageRefFromFront();
		currentMessage->open();
		switch (currentMessage->messageType)
		{
			case MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION :						{ handleRequestToClientSetWorldDirection(std::move(*currentMessage)); break; }
			case MessageType::REQUEST_FROM_SERVER_SEND_BLUEPRINTS_FOR_OGLMBUFFERMANAGER:	{ handleRequestToClientSendCurrentClientOGLMRMC(std::move(*currentMessage)); break; }
		}
		messageCableRef->popOutgoingQueue();
	}

	while (!messageCableRef->isOutgoingContainerQueueEmpty())
	{
		std::cout << "(ServerMessageInterpreter): ############### Interpreting outbound Container messages... " << std::endl;

		MessageContainer* currentContainer = messageCableRef->getOutgoingMessageContainerRefFromFront();
		switch (currentContainer->containerType)
		{
			case MessageContainerType::MC_BDM: 
			{ 
				std::cout << "Inserting BDM message back to client." << std::endl;
				serverPtr->client.insertResponseContainer(std::move(*currentContainer)); 
				break;
			}
		}
		messageCableRef->popOutgoingContainerQueue();
	}
}

void ServerMessageInterpreter::handleRequestFromClientEnableORECompositionHighlighting(Message in_message)
{
	// ####################
	// MESSAGE CHAIN: enableORECompositionHighlighting (start of chain, 2 of 3)
	//
	// Upon receiving a message of type REQUEST_FROM_CLIENT_ENABLE_ORE_COMPOSITION_HIGHLIGHTING, it has the ability to examine
	// the specified collection and ORE keys in the Message to determine what to do.

	switch (in_message.messageLocality)
	{
		// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:
		{
			std::cout << "(ServerMessageInterpreter): handling enabling ORE composition highlight request. " << std::endl;

			// ...
			// logic for reading keys and making a decision would go here (not done for now, but should be done later. Noted on 4/7/2022)
			// ...

			// just make a copy of the original message, but change it's type
			Message oreCompositionMessage = in_message;
			oreCompositionMessage.messageType = MessageType::RESPONSE_FROM_SERVER_ENABLE_ORE_COMPOSITION_HIGHLIGHTING;
			serverPtr->client.insertResponseMessage(oreCompositionMessage);

		}

		// ****************************************************************** REMOTE LOGIC
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientDisableORECompositionHighlighting(Message in_message)
{
	// ####################
	// MESSAGE CHAIN: disableORECompositionHighlighting (start of chain, 2 of 3)
	//
	// Upon receiving a message of type REQUEST_FROM_CLIENT_DISABLE_ORE_COMPOSITION_HIGHLIGHTING, the server will determine if 
	// this action is authorized, and send that result back to the corresponding OrganicSystem client.
	
	switch (in_message.messageLocality)
	{
		// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:
		{
			std::cout << "(ServerMessageInterpreter): handling disabling ORE composition highlight request. " << std::endl;

			// just make a copy of the original message, but change it's type
			Message disableMessage = in_message;
			disableMessage.messageType = MessageType::RESPONSE_FROM_SERVER_DISABLE_ORE_COMPOSITION_HIGHLIGHTING;
			serverPtr->client.insertResponseMessage(disableMessage);
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientGetBlockData(Message in_message)
{
	// ####################
	// MESSAGE CHAIN: getBlockData (2 of 3)
	//
	// Upon receiving the request from the core, the server can do a check/logic to see if it is a permitted operation.


	switch (in_message.messageLocality)
	{
		// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:
		{
			std::cout << "(ServerMessageInterpreter): handling get block data request. " << std::endl;

			// just make a copy of the original message, but change it's type
			Message fetchDataMessage = in_message;
			fetchDataMessage.messageType = MessageType::RESPONSE_FROM_SERVER_GET_BLOCK_DATA;
			serverPtr->client.insertResponseMessage(fetchDataMessage);
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientForOGLMRMCBlueprints(Message in_message)
{

	// ####################
	// MESSAGE CHAIN: serverRequestsCurrentClientOGLMRMC(3 of 3)
	//
	// The server receives a MessageType of REQUEST_FROM_CLIENT_BLUEPRINTS_FOR_OGLMBUFFERMANAGER from a client, and uses the key inside this message to construct a scanning cuboid.
	// The server checks for all blueprints that exist within the cuboid, and sends them back to the client.
	// 
	//

	switch (in_message.messageLocality)
	{
		// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:

		{
			std::cout << "Switched case found! " << std::endl;
			insertResponseToPending(in_message);			// insert into pending
			// do stuff here
			// ...
			// ...

			// extract the data
			EnclaveKeyDef::EnclaveKey extractedKey = in_message.readEnclaveKey();
			int cuboidDimension = in_message.readInt();


			std::cout << "Message key is: " << extractedKey.x << ", " << extractedKey.y << ", " << extractedKey.z << std::endl;
			std::cout << "Message cuboid dimension is: " << cuboidDimension << std::endl;

			// form a blueprint key cuboid, check each value in the array, then send over the results for everything that was found to the client
			BlueprintScanningCuboid cuboid(cuboidDimension, extractedKey);
			int cuboidArraySize = cuboid.totalCuboidElements;
			for (int x = 0; x < cuboidArraySize; x++)
			{
				EnclaveKeyDef::EnclaveKey currentKey = cuboid.keyArray[x];
				auto blueprintFinder = serverPtr->serverBlueprints.getSpecificBlueprintIter(currentKey);
				if (blueprintFinder != serverPtr->serverBlueprints.getBlueprintEndIter())	// it was found, so it needs to be sent over.
				{
					std::cout << "Key to send was found: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;

					// Apply BDM_TEST: comment below when attempting to use the new methodology of reconstitution messages.
					//serverPtr->sendAndRenderBlueprintToLocalOS(currentKey);		// toggle on/off for testing as needed (9/24/2020)

					// NEW: send data to the OrganicSystem's dedicated bluerpint processing thread,
					// (this won't do anything if it isn't running)
					//
					// Be sure to uncomment the below two lines when the time to use BDM comes!
					auto bdmFormat = serverPtr->serverBlueprints.getBlueprintRef(currentKey)->convertBlueprintTOBDMFormat(currentKey);
					messageCableRef->insertOutgoingContainer(bdmFormat);
				}
			}

			// now, tell the client to render everything in the list.
			// Apply BDM_TEST: comment below (to both lines) when attempting to use the new methodology of reconstitution messages.
			//Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_PROCESS_BLUEPRINT_WHEN_RECEIVED);
			//serverPtr->client.insertResponseMessage(responseMessage);

			moveResponseToCompleted(in_message.messageID);	// indicate that its done (if we completed, of course)
			//messageCableRef->incomingMessages.pop();
			break;
		}

		// ****************************************************************** REMOTE LOGIC
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientGetBlueprintForT1(Message in_message)
{
	// ###################
	// PREVIOUS CALL/MESSAGE: Message spawned by calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestT1Blueprint(EnclaveKeyDef::EnclaveKey in_key)
	//
	// checks to see if the blueprint specified in the message exists, and if it does, it sends the blueprint data back to the client along with a message indicating it existed.
	//
	//
	//

	switch (in_message.messageLocality)
	{
			// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:
		{
			insertResponseToPending(in_message);			// insert into pending
			EnclaveKeyDef::EnclaveKey extractedKey = in_message.readEnclaveKey();

			// do work here
			int checkResult = serverPtr->checkIfBlueprintExists(extractedKey);
			if (checkResult == 1)
			{
				serverPtr->transferBlueprintToLocalOS(extractedKey);
				Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_BLUEPRINT_T1_FOUND);
				responseMessage.insertEnclaveKey(extractedKey);
				serverPtr->client.insertResponseMessage(responseMessage);
			}

			moveResponseToCompleted(in_message.messageID);	// indicate that its done (if we completed, of course)
			//messageCableRef->incomingMessages.pop();
			break;
		}

		// ****************************************************************** REMOTE LOGIC
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientGetBlueprintForT2(Message in_message)
{
	// ###################
	// PREVIOUS CALL/MESSAGE: Message spawned by calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestT2Blueprint(EnclaveKeyDef::EnclaveKey in_key)
	//
	// checks to see if the blueprint specified in the message exists, and if it does, it sends the blueprint data back to the client along with a message indicating it existed.
	//
	//
	//

	switch (in_message.messageLocality)
	{
			// ****************************************************************** LOCAL LOGIC
		case MessageLocality::LOCAL:
		{
			insertResponseToPending(in_message);			// insert into pending
			EnclaveKeyDef::EnclaveKey extractedKey = in_message.readEnclaveKey();

			// do work here
			int checkResult = serverPtr->checkIfBlueprintExists(extractedKey);
			if (checkResult == 1)
			{
				serverPtr->transferBlueprintToLocalOS(extractedKey);
				Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_BLUEPRINT_T2_FOUND);
				//std::cout << "SERVER: found T2 blueprint (" << extractedKey.x << ", " << extractedKey.y << ", " << extractedKey.z << ") " << std::endl;
				responseMessage.insertEnclaveKey(extractedKey);
				serverPtr->client.insertResponseMessage(responseMessage);
			}
			moveResponseToCompleted(in_message.messageID);	// indicate that its done (if we completed, of course)
			//messageCableRef->incomingMessages.pop();
			break;
		}
		// ****************************************************************** REMOTE LOGIC
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientRunContourPlan(Message in_message)
{
	// other cases follow here...
	// ###################
	//
	// MESSAGE CHAIN: clientRequestsContourPlanRun(message sent to serverJobManager, 2 of 3)
	// PREVIOUS CALL/MESSAGE: client requests a contour plan via button click in ImGui.
	//

	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "!! Found contour map request. " << std::endl;
			serverPtr->serverJobManager.insertJobRequestMessage(std::move(in_message));
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestToClientSetWorldDirection(Message in_message)
{
	// other cases follow here...
	// ###################
	// PREVIOUS CALL/MESSAGE: client requests a contour plan via button click in ImGui.
	//
	//
	//

	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: sending request to client to set world direction" << std::endl;
			serverPtr->client.insertResponseMessage(in_message);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestToClientSendCurrentClientOGLMRMC(Message in_message)
{
	// ###################
	//
	// MESSAGE CHAIN: serverRequestsCurrentClientOGLMRMC (message sent to serverJobManager, 1 of 3)
	// 
	// the server needs to send a request to all clients, to get the center key of the OGLMRMC.
	//
	// will eventually need logic below to run the check against any connected client.

	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: sending request for current OGLM center key value." << std::endl;
			serverPtr->client.insertResponseMessage(in_message);
			break;
		}
		case MessageLocality::REMOTE:
		{
		

			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientToggleBlockHighlighting(Message in_message)
{
	// other cases follow here...
	// ###################
	// MESSAGE CHAIN: toggleBlockHighlighting (end of chain, 2 of 2)
	//
	// PREVIOUS CALL/MESSAGE: client requests to turn on/off current target block highlighting; this request is generated by CoreMessageInterpreter::interpretOutgoingRequestToServer;
	// see the case for MessageType::REQUEST_FROM_CLIENT_TOGGLE_BLOCK_HIGHLIGHTING in CoreMessageInterpreter::sendMessageRequestToggleBlockHighlighting() (OrganicCoreLib)
	//
	//
	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: sending toggle block highlighting requested back to client..." << std::endl;
			Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_TOGGLE_BLOCK_HIGHLIGHTING);
			serverPtr->client.insertResponseMessage(responseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientToggleCurrentEnclaveHighlighting(Message in_message)
{
	// ###################
	// MESSAGE CHAIN: toggleBlockHighlighting (end of chain, 2 of 2)
	//
	// PREVIOUS CALL/MESSAGE: client requests to turn on/off current target block highlighting; this request is generated by CoreMessageInterpreter::interpretOutgoingRequestToServer;
	// see the case for MessageType::REQUEST_FROM_CLIENT_TOGGLE_BLOCK_HIGHLIGHTING in CoreMessageInterpreter::sendMessageRequestToggleBlockHighlighting() (OrganicCoreLib)
	//
	//
	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: sending toggle current enclave highlighting requested back to client..." << std::endl;
			Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_TOGGLE_CURRENT_ENCLAVE_HIGHLIGHTING);
			serverPtr->client.insertResponseMessage(responseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientOGLMToggleWorldLocation(Message in_message)
{
	std::cout << "SERVER: Calling handleRequestFromClientOGLMToggleWorldLocation..." << std::endl;
	// ####################
	// MESSAGE CHAIN: toggleWorldLocationPanel (2 of 2)
	//
	// This sends a response message back to the requesting OrganicSystem instance -- local or remote host -- that requested the toggling of the World Location panel in the GUI.
	//
	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: sending OK for toggling of World Location panel back to client..." << std::endl;
			Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_TOGGLE_IMGUI_WORLD_LOCATION);
			serverPtr->client.insertResponseMessage(responseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientInputGenerated(Message in_message)
{
	std::cout << "SERVER: Calling handleRequestFromClientInputGenerated..." << std::endl;
	// ####################
	// MESSAGE CHAIN: clientSendsInput  (2 of 2)
	//
	// This sends a response message back to the requesting OrganicSystem instance -- local or remote host -- indicating whether or not the input was valid/allowed.
	//
	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: checking message contents for input received: " << std::endl;
			in_message.open();

			// For later on, we could individually analyze each input to see if their valid, and return
			// a REFUSED message if any are not allowed.
			int numberOfInputs = in_message.readInt();
			for (int x = 0; x < numberOfInputs; x++)
			{
				int currentInput = in_message.readInt();
				std::cout << "GLFW input enum value: " << currentInput << std::endl;
			}

			// Read the last int, which would be the pending action ID used for this Message, as it was generated by the call to
			// InputCycler::getNextActionID(), in the function CoreMessageInterpreter::sendMessageRequestInputReceived.
			int pendingActionID = in_message.readInt();
			std::cout << "SERVER: received pending action ID for this input is: " << pendingActionID << std::endl;

			// our response message will be a copy of th request message, the only difference being that the response message
			// has its MessageType changed to RESPONSE_FROM_SERVER_INPUT_GENERATED, and includes a response string.
			std::cout << "SERVER: sending OK for input received back to client..." << std::endl;
			Message generatedResponseMessage = in_message;
			generatedResponseMessage.insertString("ACCEPTED");
			generatedResponseMessage.messageType = MessageType::RESPONSE_FROM_SERVER_INPUT_GENERATED;

			//Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_INPUT_GENERATED);
			serverPtr->client.insertResponseMessage(generatedResponseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientMouseWheelScroll(Message in_message)
{
	std::cout << "SERVER: Calling handleRequestFromClientMouseWheelScroll..." << std::endl;
	// ####################
	// MESSAGE CHAIN: clientSendsMouseScroll (2 of 2)
	//
	// This sends a response message back to the requesting OrganicSystem instance, indicating whether or not the mouse scroll input
	// was approved. We must send back the input request's unique ID (which relates to an int value in either the positiveMouseScrollMessages or 
	// negativeMouseScrollMessages set in InputDirector)

	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: checking message contents for mouse scroll input received: " << std::endl;
			in_message.open();
			int inputUniqueID = in_message.readInt();
			float posOrNeg = in_message.readFloat();
			Message responseMessage(MessageType::RESPONSE_FROM_SERVER_MOUSE_SCROLL_INPUT_GENERATED);
			responseMessage.insertInt(inputUniqueID);
			responseMessage.insertFloat(posOrNeg);
			serverPtr->client.insertResponseMessage(responseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}

void ServerMessageInterpreter::handleRequestFromClientBlockDelete(Message in_message)
{
	std::cout << "SERVER: Calling handleRequestFromClientBlockDelete..." << std::endl;
	// ####################
	// MESSAGE CHAIN: clientRequestsBlockDelete (2 of 3)
	//
	// The server can read the message, and check if the requested block to delete is a valid request;
	// an example of an invalid request would be the if the client wants to request the delete of a block,
	// but the server doesn't allow that specific block to be modified (this is just an example; this logic has not been
	// implemented yet as of 4/3/2022)

	switch (in_message.messageLocality)
	{
		case MessageLocality::LOCAL:
		{
			std::cout << "SERVER: checking if requested delete is valid. " << std::endl;
			in_message.open();

			// ...logic to read and verify keys would go here.
			//
			// ...just assume it's good for now and come back to this later.
			Message responseMessage = in_message;
			responseMessage.messageType = MessageType::RESPONSE_FROM_SERVER_REMOVE_BLOCK;
			serverPtr->client.insertResponseMessage(responseMessage);
			break;
		}
		case MessageLocality::REMOTE:
		{
			break;
		}
	}
}
