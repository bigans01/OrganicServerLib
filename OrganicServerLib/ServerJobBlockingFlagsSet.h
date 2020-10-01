#pragma once

#ifndef SERVERJOBBLOCKINGFLAGSSET_H
#define SERVERJOBBLOCKINGFLAGSSET_H

#include <set>
#include <mutex>
#include "ServerJobBlockingFlags.h"

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


	private:
		std::mutex flagsGuard;
		std::set<ServerJobBlockingFlags> flags;
};

#endif
