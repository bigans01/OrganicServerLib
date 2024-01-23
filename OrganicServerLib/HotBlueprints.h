#pragma once

#ifndef HOTBLUEPRINTS_H
#define HOTBLUEPRINTS_H

#include <unordered_map>
#include <set>
#include <unordered_set>
#include "EnclaveKeyDef.h"
#include "Operable3DEnclaveKeySet.h"

/*

Description: the HotBlueprints class is intended to serve as a means to determine which blueprints are currently undergoing
some sort of modification in the ECBMap that belongs to the OrganicServer instance (see hotBPManager member, in OSServer class)

Keep in mind, multiple types of operations may seek to modify this class at the same time; for instance, block modification operations
would seek to acquire a lock on a particular blueprint, which should be fine as long as the current contour plan doesn't intend to 
access that same blueprint. 

*/

class HotBlueprints
{
	public:
		
		void insertKeyIntoPillar(EnclaveKeyDef::EnclaveKey in_key)
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			EnclaveKeyDef::Enclave2DKey currentXZ(in_key.x, in_key.z);
			blueprintPillars[currentXZ].insert(in_key.y);
		}

		// Below: claer out the hotKeys map, in it's entirety.
		void clearhotKeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			hotKeys.clear();
		}

		int gethotKeysize()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			return hotKeys.size();
		}

		// Below: produce the keys from pillars, and insert them into the hotKeys. Should be called before a CPV2 does its run,
		// so that other activities running on the server that would be using the CPV2's potential affected blueprints 
		// can determine whether they must wait/cancel themselves.
		void produceKeysFromPillars()
		{
			std::lock_guard<std::mutex> lock(hotGuard);

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
					hotKeys += newKey;
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
						hotKeys += newKey;
					}
				}
			}
		}

		// Below: produce the keys from pillars, but delete them from the hotkeys. Should be called when a CPV2 is done
		// doing its run, so that the keys can be modified by other activity in the OrganicServer.
		void deletePillarKeysFromHotkeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
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
					hotKeys += newKey;
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
						hotKeys += newKey;
					}
				}
			}
		}

		void printPillarKeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);

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

		Operable3DEnclaveKeySet hotKeys;	// any key that is in this set should be considered "hot", as in, it is currently being read/modified/erased, etc.

	private:
		std::mutex hotGuard;	// intended for thread-safe operations; should be used in a lock_guard whenever reading/modifying/erasing members of this class.
		std::unordered_map<EnclaveKeyDef::Enclave2DKey, std::set<int>, EnclaveKeyDef::KeyHasher> blueprintPillars;
};

#endif
