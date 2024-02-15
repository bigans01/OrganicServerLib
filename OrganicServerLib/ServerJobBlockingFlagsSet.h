#pragma once

#ifndef SERVERJOBBLOCKINGFLAGSSET_H
#define SERVERJOBBLOCKINGFLAGSSET_H

#include <set>
#include <mutex>
#include "ServerJobBlockingFlags.h"

/*

Description: A simple, thread-safe flag management class that is designed for the ServerJobManager class

*/

class ServerJobBlockingFlagsSet
{
	public:
		void insertFlag(ServerJobBlockingFlags in_flag)
		{
			std::lock_guard<std::mutex> lock(flagsGuard);
			flags.insert(in_flag);
		}

		void eraseFlag(ServerJobBlockingFlags in_flag)
		{
			std::lock_guard<std::mutex> lock(flagsGuard);
			flags.erase(in_flag);
		}

		bool checkIfFlagExists(ServerJobBlockingFlags in_flag)
		{
			std::lock_guard<std::mutex> lock(flagsGuard);
			bool returnValue = false;
			auto flagFinder = flags.find(in_flag);
			if (flagFinder != flags.end())
			{
				returnValue = true;
			}
			return returnValue;
		}

		bool attemptFlagRaise(ServerJobBlockingFlags in_flag)
		{
			std::lock_guard<std::mutex> lock(flagsGuard);

			bool wasRaiseSuccessful = false;

			// check that the flag DOESN'T exist; if it doesn't, 
			// raise/insert it 
			auto flagFinder = flags.find(in_flag);
			if (flagFinder == flags.end())
			{
				flags.insert(in_flag);
				wasRaiseSuccessful = true;
			}

			return wasRaiseSuccessful;
		}


	private:
		std::mutex flagsGuard;
		std::set<ServerJobBlockingFlags> flags;
};

#endif
