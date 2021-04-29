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
		}
		messageCableRef->popIncomingQueue();
	}
}

void ServerMessageInterpreter::interpretOutgoingMessagesToClient()
{
	while (!messageCableRef->isOutgoingQueueEmpty())
	{
		std::cout << "############### Interpreting outbound messages... " << std::endl;
		Message* currentMessage = messageCableRef->getOutgoingMessageRefFromFront();
		currentMessage->open();
		switch (currentMessage->messageType)
		{
			case MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION :						{ handleRequestToClientSetWorldDirection(std::move(*currentMessage)); break; }
			case MessageType::REQUEST_FROM_SERVER_SEND_BLUEPRINTS_FOR_OGLMBUFFERMANAGER:	{ handleRequestToClientSendCurrentClientOGLMRMC(std::move(*currentMessage)); break; }
		}
		messageCableRef->popOutgoingQueue();
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
				auto blueprintFinder = serverPtr->blueprintMap.find(currentKey);
				if (blueprintFinder != serverPtr->blueprintMap.end())	// it was found, so it needs to be sent over.
				{
					std::cout << "Key to send was found: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;
					serverPtr->sendAndRenderBlueprintToLocalOS(currentKey);		// toggle on/off for testing as needed (9/24/2020)
				}
			}

			// now, tell the client to render everything in the list.
			Message responseMessage(in_message.messageID, in_message.messageLocality, MessageType::RESPONSE_FROM_SERVER_PROCESS_BLUEPRINT_WHEN_RECEIVED);
			serverPtr->client.insertResponseMessage(responseMessage);

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

