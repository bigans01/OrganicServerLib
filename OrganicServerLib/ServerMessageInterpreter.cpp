#include "stdafx.h"
#include "ServerMessageInterpreter.h"
#include "OSServer.h"

void ServerMessageInterpreter::initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef)
{
	serverPtr = in_organicSystemServerRef;
	messageCableRef = in_messageCableRef;
}

void ServerMessageInterpreter::interpretIncomingRequestsFromClient()	// for interpreting inbound requests from a client
{
	while (!messageCableRef->incomingMessages.empty())
	{
		//std::cout << "Checking incoming requests from client: " << std::endl;
		Message* currentMessage = &messageCableRef->incomingMessages.front();
		currentMessage->open();					// open the message for reading (sets the iterators)
		switch (currentMessage->messageType)											
		{
			
			// ###################
			// PREVIOUS CALL/MESSAGE: Message spawned by calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestAllBlueprintsInOGLMRMC(EnclaveKeyDef::EnclaveKey in_OGLMCenterKey)
			//
			// uses a BlueprintScanningCuboid that is constructed based off the client's current center collection key, to scan for existing blueprints. The blueprints are then sent the client.
			//
			//
			//
			case MessageType::REQUEST_FROM_CLIENT_BLUEPRINTS_FOR_OGLMBUFFERMANAGER :
			{
				switch (currentMessage->messageLocality)
				{
					// ****************************************************************** LOCAL LOGIC
					case MessageLocality::LOCAL :

					{
						std::cout << "Switched case found! " << std::endl;
						insertResponseToPending(*currentMessage);			// insert into pending
						// do stuff here
						// ...
						// ...

						// extract the data
						EnclaveKeyDef::EnclaveKey extractedKey = currentMessage->readEnclaveKey();
						int cuboidDimension = currentMessage->readInt();


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
						//serverPtr->client.callMaterializeAllCollectionsInRenderListFromMessage();

						// version 2.0
						Message responseMessage(currentMessage->messageID, currentMessage->messageLocality, MessageType::RESPONSE_FROM_SERVER_PROCESS_BLUEPRINT_WHEN_RECEIVED);
						serverPtr->client.insertResponseMessage(responseMessage);

						moveResponseToCompleted(currentMessage->messageID);	// indicate that its done (if we completed, of course)
						//messageCableRef->incomingMessages.pop();
						break;
					}

					// ****************************************************************** REMOTE LOGIC
					case MessageLocality::REMOTE :
					{
						break;
					}
				}
				break;
			}

			// ###################
			// PREVIOUS CALL/MESSAGE: Message spawned by calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestT1Blueprint(EnclaveKeyDef::EnclaveKey in_key)
			//
			// checks to see if the blueprint specified in the message exists, and if it does, it sends the blueprint data back to the client along with a message indicating it existed.
			//
			//
			//
			case MessageType::REQUEST_FROM_CLIENT_GET_BLUEPRINT_FOR_T1 :
			{
				switch (currentMessage->messageLocality)
				{
					// ****************************************************************** LOCAL LOGIC
					case MessageLocality::LOCAL:
					{
						insertResponseToPending(*currentMessage);			// insert into pending
						EnclaveKeyDef::EnclaveKey extractedKey = currentMessage->readEnclaveKey();

						// do work here
						int checkResult = serverPtr->checkIfBlueprintExists(extractedKey);
						if (checkResult == 1)
						{
							serverPtr->transferBlueprintToLocalOS(extractedKey);
							Message responseMessage(currentMessage->messageID, currentMessage->messageLocality, MessageType::RESPONSE_FROM_SERVER_BLUEPRINT_T1_FOUND);
							responseMessage.insertEnclaveKey(extractedKey);
							serverPtr->client.insertResponseMessage(responseMessage);
						}

						moveResponseToCompleted(currentMessage->messageID);	// indicate that its done (if we completed, of course)
						//messageCableRef->incomingMessages.pop();
						break;
					}

					// ****************************************************************** REMOTE LOGIC
					case MessageLocality::REMOTE:
					{
						break;
					}
				}
				break;
			}


			// ###################
			// PREVIOUS CALL/MESSAGE: Message spawned by calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestT2Blueprint(EnclaveKeyDef::EnclaveKey in_key)
			//
			// checks to see if the blueprint specified in the message exists, and if it does, it sends the blueprint data back to the client along with a message indicating it existed.
			//
			//
			//
			case MessageType::REQUEST_FROM_CLIENT_GET_BLUEPRINT_FOR_T2:
			{
				switch (currentMessage->messageLocality)
				{
					// ****************************************************************** LOCAL LOGIC
					case MessageLocality::LOCAL:
					{
						insertResponseToPending(*currentMessage);			// insert into pending
						EnclaveKeyDef::EnclaveKey extractedKey = currentMessage->readEnclaveKey();

						// do work here
						int checkResult = serverPtr->checkIfBlueprintExists(extractedKey);
						if (checkResult == 1)
						{
							serverPtr->transferBlueprintToLocalOS(extractedKey);
							Message responseMessage(currentMessage->messageID, currentMessage->messageLocality, MessageType::RESPONSE_FROM_SERVER_BLUEPRINT_T2_FOUND);
							std::cout << "SERVER: found T2 blueprint (" << extractedKey.x << ", " << extractedKey.y << ", " << extractedKey.z << ") " << std::endl;
							responseMessage.insertEnclaveKey(extractedKey);
							serverPtr->client.insertResponseMessage(responseMessage);
						}
						moveResponseToCompleted(currentMessage->messageID);	// indicate that its done (if we completed, of course)
						//messageCableRef->incomingMessages.pop();
						break;
					}
					// ****************************************************************** REMOTE LOGIC
					case MessageLocality::REMOTE:
					{
						break;
					}
				}
				break;
			}

			// other cases follow here...
			// ###################
			// PREVIOUS CALL/MESSAGE: client requests a contour plan via button click in ImGui.
			//
			//
			//
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN:
			{
				switch (currentMessage->messageLocality)
				{
					case MessageLocality::LOCAL:
					{
						std::cout << "!! Found contour map request. " << std::endl;
						serverPtr->serverJobManager.messageQueue.insertMessage(std::move(*currentMessage));		//  move the request to the server job manager.
						break;
					}
					case MessageLocality::REMOTE:
					{
						break;
					}
				}
				break;
			}
		}
		messageCableRef->incomingMessages.pop();
	}
}

void ServerMessageInterpreter::interpretIncomingResponsesFromClient()
{

}