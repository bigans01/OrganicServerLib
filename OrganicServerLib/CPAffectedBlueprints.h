#pragma once

#ifndef CPAFFECTEDBLUEPRINTS_H
#define CPAFFECTEDBLUEPRINTS_H

#include <unordered_map>
#include <set>
#include <unordered_set>
#include "EnclaveKeyDef.h"

class CPAffectedBlueprints
{
	public:
		void insertKeyIntoPillar(EnclaveKeyDef::EnclaveKey in_key)
		{
			EnclaveKeyDef::Enclave2DKey currentXZ(in_key.x, in_key.z);
			blueprintPillars[currentXZ].insert(in_key.y);
		}

		void clearProducedKeys()
		{
			producedKeys.clear();
		}

		int getProducedKeySize()
		{
			return producedKeys.size();
		}

		void produceKeys()
		{
			// Remember, the current value of pillarsBegin is a 2D key that stores the X (in a) 
			// and Z (in b) values.
			auto pillarsBegin = blueprintPillars.begin();
			auto pillarsEnd = blueprintPillars.end();
			for (; pillarsBegin != pillarsEnd; pillarsBegin++)
			{
				if (pillarsBegin->second.size() == 1)	// if it's just one, that's fine; insert it into the map.
				{
					// There would only be one Y-value for the pillar that this key is in.
					auto pillarKey = pillarsBegin->first;	// get the X and Z values
					int yValueToUse = *(pillarsBegin->second).begin();
					EnclaveKeyDef::EnclaveKey newKey(pillarKey.a, yValueToUse, pillarKey.b);
					producedKeys.insert(newKey);
				}
				else
				{
					// Need to generate a key for each value; grab the first element via begin, and the last element via rbegin.
					// Since the std::set is ordered, the number of times to iterate is equal to the (last element - the begin element) + 1.
					auto yBegin = *(pillarsBegin->second).begin();
					auto yEnd = *(pillarsBegin->second).rbegin();
					int totalIterations = yEnd - yBegin;	// remember, std::set is ordered from least to greatest
					for (int x = yBegin; x < (yEnd + 1); x++)
					{
						auto pillarKey = pillarsBegin->first;	// get the X and Z values
						int yValueToUse = x;
						EnclaveKeyDef::EnclaveKey newKey(pillarKey.a, yValueToUse, pillarKey.b);
						producedKeys.insert(newKey);
					}
				}
			}
		}

		void printPillarKeys()
		{
			auto pillarsBegin = blueprintPillars.begin();
			auto pillarsEnd = blueprintPillars.end();
			for (; pillarsBegin != pillarsEnd; pillarsBegin++)
			{
				if (pillarsBegin->second.size() == 1)	// if it's just one, that's fine; insert it into the map.
				{
					// There would only be one Y-value for the pillar that this key is in.
					auto pillarKey = pillarsBegin->first;	// get the X and Z values
					std::cout << "Pillar: (" << pillarKey.a << ", " << pillarKey.b << ") -> ";	// output the current pillar's X/Z
					int yValueToUse = *(pillarsBegin->second).begin();
					EnclaveKeyDef::EnclaveKey newKey(pillarKey.a, yValueToUse, pillarKey.b);
					std::cout << "(" << newKey.x << ", " << newKey.y << ", " << newKey.z << ") " << std::endl;
				}
				else
				{
					auto pillarKey = pillarsBegin->first;	// get the X and Z values

					// Need to generate a key for each value; grab the first element via begin, and the last element via rbegin.
					// Since the std::set is ordered, the number of times to iterate is equal to the (last element - the begin element) + 1.
					auto yBegin = *(pillarsBegin->second).begin();
					auto yEnd = *(pillarsBegin->second).rbegin();

					std::cout << "Pillar: (" << pillarKey.a << ", " << pillarKey.b << ") -> ";	// output the current pillar's X/Z
					int totalIterations = yEnd - yBegin;	// remember, std::set is ordered from least to greatest
					for (int x = yBegin; x < (yEnd + 1); x++)
					{
						int yValueToUse = x;	// the y value to use for this iteration
						EnclaveKeyDef::EnclaveKey newKey(pillarKey.a, yValueToUse, pillarKey.b);
						std::cout << "(" << newKey.x << ", " << newKey.y << ", " << newKey.z << ")";
						if (x < yEnd)
						{
							std::cout << ", ";
						}
					}
					std::cout << std::endl;
				}
			}
		}

		std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> producedKeys;
	private:
		std::unordered_map<EnclaveKeyDef::Enclave2DKey, std::set<int>, EnclaveKeyDef::KeyHasher> blueprintPillars;
};

#endif
