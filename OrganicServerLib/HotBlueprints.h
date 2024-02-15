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

		void buildRequiredCPV2Keys()	// determines and inserts the estimated key values a CPV2 will attempt to use,
										// and loads them into the pillarCPV2Keys set.
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
					pillarCPV2Keys += newKey;
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
						pillarCPV2Keys += newKey;
					}
				}
			}
		}

		bool attemptLoadOfCPV2KeysIntoHotKeys()	// assuming it contains data, move the contents of pillarCPV2Keys into hotKeys,
												// to signal that the CPV2 keys are now flagged as "hot"
		{
			// Must lock while attempting to do this, since we will both check if the keys the CPV2 needs
			// are available, and then lock them if so.
			std::lock_guard<std::mutex> lock(hotGuard);

			// Set to true if the 
			bool loadSucceeded = false;

			// Check if any of the pillar keys are currently in the hotKeys;
			// if any are found, return false
			for (auto& currentPillarKey : pillarCPV2Keys)
			{
				auto keySearchResult = hotKeys.find(currentPillarKey);

				// if, at any point, an estimated key for the CPV2 is already found in hotkeys, 
				// it means some other job/work is utilizing it, and therefore the load attempt should fail.
				if (keySearchResult != hotKeys.end())
				{
					loadSucceeded = true;
					return loadSucceeded;
				}
			}

			// If we got here, we haven't found any CPV2 keys in hotkeys. So, go ahead and load the pillarCPV2Keys into the
			// hotKeys.
			hotKeys += pillarCPV2Keys;

			return loadSucceeded;
		}

		void unloadCPV2KeysFromHotkeys()
		{
			// Must lock while attempting to unload.
			std::lock_guard<std::mutex> lock(hotGuard);
			hotKeys -= pillarCPV2Keys;
		}

		// Below: append the contents from pillarCPV2Keys into hotKeys.
		// Used by SJs such as SJBuildContourPlanAffectedBlueprints to ensure that the blueprints a CPV2 is about to potentially 
		// update have been flagged as "hot"
		void appendPillarKeysToHotkeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			hotKeys += pillarCPV2Keys;
		}

		// Below: remove the keys found in the pillarCPV2Keys member, from the hotKeys. Should be called when a CPV2 is done
		// doing its run, so that the keys can be modified by other activity in the OrganicServer. Used by the SJ,
		// SJCleanupCPV2 through a call to ServerJobProxy.
		void deletePillarKeysFromHotkeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			hotKeys -= pillarCPV2Keys;
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

		Operable3DEnclaveKeySet fetchPillarKeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			return pillarCPV2Keys;
		}

		Operable3DEnclaveKeySet fetchHotKeys()
		{
			std::lock_guard<std::mutex> lock(hotGuard);
			return hotKeys;
		}


	private:
		std::mutex hotGuard;	// intended for thread-safe operations; should be used in a lock_guard whenever reading/modifying/erasing members of this class.
		std::unordered_map<EnclaveKeyDef::Enclave2DKey, std::set<int>, EnclaveKeyDef::KeyHasher> blueprintPillars;

		Operable3DEnclaveKeySet hotKeys;	// any key that is in this set should be considered "hot", as in, it is currently being read/modified/erased, etc.
		Operable3DEnclaveKeySet pillarCPV2Keys;	// any keys that a currently active CPV2 would need during its processing, to guarantee no interference from outside work, 
												// will go here.
};

#endif
