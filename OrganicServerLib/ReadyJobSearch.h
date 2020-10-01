#pragma once

#ifndef READYJOBSEARCH_H
#define READYJOBSEARCH_H

#include <mutex>
#include "ServerJobBase.h"

class ReadyJobSearch
{
	public:
		bool wasJobFound = false;
		std::shared_ptr<ServerJobBase>* currentJobPtr = NULL;	// default is null
};

#endif

