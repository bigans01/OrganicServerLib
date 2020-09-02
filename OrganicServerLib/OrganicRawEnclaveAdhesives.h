#pragma once

#ifndef ORGANICRAWENCLAVEADHESIVES_H
#define ORGANICRAWENCLAVEADHESIVES_H

#include "OrganicRawEnclave.h"
#include "EuclideanDirection3D.h"
#include <vector>

class OrganicRawEnclaveAdhesives
{
	public:
		
		OrganicRawEnclaveAdhesives() {}
		OrganicRawEnclaveAdhesives(OrganicRawEnclave* in_organicRawEnclaveRef, EuclideanDirection3D in_euclideanDirection) //: organicRawEnclaveRef(in_organicRawEnclaveRef)
		{
			//adhesiveDirections.push_back(in_euclideanDirection);	// when initialize, push back the first value.
		}
		void insertDirection(EuclideanDirection3D in_euclideanDirection)
		{
			//adhesiveDirections.push_back(in_euclideanDirection);
		}
		

		OrganicRawEnclave* organicRawEnclaveRef;
		//int someVal = 3;
		//std::vector<EuclideanDirection3D> adhesiveDirections;

	private:
		//OrganicRawEnclave* organicRawEnclaveRef;
		//std::vector<EuclideanDirection3D> adhesiveDirections;
		//std::vector<int> someVec;
};

#endif
