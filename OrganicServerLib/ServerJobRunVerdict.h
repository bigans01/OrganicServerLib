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

		void setTruthAndDesignatedString(std::string in_string)
		{
			canJobRun = true;
			designatedThreadString = in_string;
		}

		bool canJobRun = false;
		std::string designatedThreadString = "";
};

#endif
