#pragma once

#ifndef SERVERJOBRUNVERDICT_H
#define SERVERJOBRUNVERDICT_H

#include <string>

class ServerJobRunVerdict
{
	public:
		ServerJobRunVerdict() {};
		ServerJobRunVerdict(bool in_canJobRun, std::string in_designatedThreadString) :
			canJobRun(in_canJobRun),
			designatedThreadString(in_designatedThreadString)
		{};

		bool canJobRun = false;
		std::string designatedThreadString = "";
};

#endif
