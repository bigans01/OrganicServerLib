#include "stdafx.h"
#include "ServerJobRunCounters.h"

int ServerJobRunCounters::getCategoryCount(ServerJobRunCategory in_category)
{
	std::lock_guard<std::mutex> guard(flagsGuard);
	auto categoryFinder = runCategoryMap.find(in_category);
	if (categoryFinder != runCategoryMap.end())	// it was found
	{
		return runCategoryMap[in_category];
	}
	else
	{
		return 0;
	}
}

void ServerJobRunCounters::decrementCategory(ServerJobRunCategory in_category)
{
	std::lock_guard<std::mutex> guard(flagsGuard);
	runCategoryMap[in_category]--;
	if (runCategoryMap[in_category] == 0)
	{
		// erase if we don't need it, clean up space
		runCategoryMap.erase(in_category);
	}
}

void ServerJobRunCounters::incrementCategory(ServerJobRunCategory in_category)
{
	std::lock_guard<std::mutex> guard(flagsGuard);
	runCategoryMap[in_category]++;
}

bool ServerJobRunCounters::isCategoryAllZero(ServerJobRunCategory in_category)
{
	std::lock_guard<std::mutex> guard(flagsGuard);
	bool returnValue = false;
	auto categoryFinder = runCategoryMap.find(in_category);
	if (categoryFinder != runCategoryMap.end())	// it was found
	{
		if (categoryFinder->second == 0)		// all zero
		{
			returnValue = true;
		}
		else if (categoryFinder->second > 0)	// anything but zero
		{
			returnValue = false;
		}
	}
	else // it wasn't found, therefore the count is assumed to be 0.
	{
		returnValue = true;
	}
	return returnValue;
}