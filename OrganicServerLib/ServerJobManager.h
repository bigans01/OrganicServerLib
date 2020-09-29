#pragma once

#ifndef SERVERJOBMANAGER_H
#define SERVERJOBMANAGER_H

#include "ServerThreadDesignationMap.h"

class OSServer;
class ServerJobManager
{
	public:
		OSServer* server;	// a pointer to the server instance that this ServerJobManager instance should be contained in.
		ServerThreadDesignationMap designations;

		void initialize(OSServer* in_serverPtr);
		void startCommandLine();
};

#endif
