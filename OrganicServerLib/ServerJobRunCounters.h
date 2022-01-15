#pragma once

#ifndef SERVERJOBRUNCOUNTERS_H
#define SERVERJOBRUNCOUNTERS_H

#include "ServerJobRunCategory.h"
#include <map>
#include <mutex>

class ServerJobRunCounters
{
	public:
		int getCategoryCount(ServerJobRunCategory in_category);
		bool isCategoryAllZero(ServerJobRunCategory in_category);
		void decrementCategory(ServerJobRunCategory in_category);
		void incrementCategory(ServerJobRunCategory in_category);
	private:
		std::mutex flagsGuard;
		std::map<ServerJobRunCategory, int> runCategoryMap;
};

#endif
