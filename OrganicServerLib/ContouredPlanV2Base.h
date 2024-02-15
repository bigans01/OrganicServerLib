#pragma once

#ifndef CONTOUREDPLANV2BASE_H
#define CONTOUREDPLANV2BASE_H

#include "ContouredTriangleV2.h"
#include "ForgedPolyRegistry.h"
#include "OrganicClient.h"
#include "CTV2Strip.h"
#include "ECBMap.h"
#include "OrganicTriangleTracker.h"
#include "ContouredPlanUtils.h"
#include "HotBlueprints.h"

class ContouredPlanV2Base
{
	public:
		// template function for inserting preferred materials
		template<typename FirstMaterial, typename ...RemainingMaterials> void insertMaterials(FirstMaterial && first, RemainingMaterials && ...remaining)
		{
			insertPreferredMaterial(OSTriangleMaterialLookup::findMaterialID(std::forward<FirstMaterial>(first)));
			insertMaterials(std::forward<RemainingMaterials>(remaining)...);
		}
		void insertMaterials() {};



		void copyOverProducedECBPolys(std::vector<ContouredTriangleV2*> in_ctv2Vector, ECBMap* in_blueprintMapRef);	// produce the estimated affected blueprints that this plan will affect.

		// Below: functions the same ascopyOverProducedECBPolys, 
		// with the exception that backups of blueprints are made from a set of blueprints
		// that we think will be affected (this is done using the referenced HotBlueprints object).
		// Should be used when a server phased job is used to build and run a contour plan,
		// instead of copyOverProducedECBPolys.
		void copyOverForSPJ(std::vector<ContouredTriangleV2*> in_ctv2Vector,	
							ECBMap* in_blueprintMapRef,
							ECBMap* in_backupBlueprintsMapRef,
							HotBlueprints* in_trackedBlueprintsRef);

		// Below: cycles through all ContouredTriangleV2 instances of the plan, fractures the FTriangle
		// equivalent of those instancse, and fetches the unique blueprint keys produced by them, in order
		// to feed them into the referenced HotBlueprints. A call is then made to buildRequiredCPV2Keys and appendPillarKeysToHotkeys() on that object,
		// to determine the estimated affected blueprints, and flag them as "hot". Used by the function copyOverForSPJ, in order
		// to facilitate the creation of backup copies of blueprints that are about to be modified, when an SPJ
		// that builds/executes a contour plan runs.
		void constructHotEstimatedAffectedBlueprints(std::vector<ContouredTriangleV2*> in_ctv2Vector, HotBlueprints* in_trackedBlueprintsRef);

		// Below: functions identically to constructHotEstimatedAffectedBlueprints, with the exception that appendPillarKeysToHotkeys is not called.
		// Utilized by OSServer::determineAffectedBlueprintsForCPV2, and ultimately called through the SJ, SJDetermineAffectedBlueprints -- which
		// calls callDetermineAffectedBlueprintsForCPV2.
		void determineEstimatedAffectedBlueprints(std::vector<ContouredTriangleV2*> in_ctv2Vector, HotBlueprints* in_trackedBlueprintsRef);

		bool wasPlanSuccessful();

		// |||||||||||||||||||||||||||| public virtual functions
		virtual void initialize(DoublePoint in_startPoint,			// Step 1: initializer function that must be called once the derived class is instantiated.
								int in_numberOfLayers,
								float in_distanceBetweenLayers,
								float in_startRadius, 
								float in_expansionValue) = 0;

		virtual void initializeFromMessage(Message in_messageToInitFrom) = 0;	// Allows each derived class of the base class to use a Message object
																				// to store data needed for the specific derived-object to run. (i.e, number of mountain layers for a CPV2 mountain, disttance between layers, etc)

		virtual void amplifyAllContourLinePoints() = 0;				// Step 2: amplification function 
		virtual void buildContouredTriangles() = 0;					// Step 3: build out the contoured triangles.

		virtual std::vector<ContouredTriangleV2*> getProcessableContouredTriangles() = 0;	// Step 4:	fetch a vector of all produced triangles,
																				//			and process them one-by-one; the fracture()
																				//			function will have to be called on each instance,
																				//			and then the fetchOutputContainerRef() will need to be called.
																				//			The return value of that function should contain the map of 
																				//			FTriangleOutput instances that would contain the ECBPolys to use.

		virtual void runMassDriversV2(OrganicClient* in_clientRef,
			ECBMap* in_ecbMapRef,
			EnclaveFractureResultsMap* in_fractureResultsMapRef) = 0;	// Step 5:	Once the child plan has applied the resulting ECBPolys of the FTriangleOutput instances 
																		//			to their respective blueprints, we can attempt mass driving.


		ForgedPolyRegistry allPolysRegistry;		// all produced ECBPolys from ContouredTriangleV2 instances initially get a pointer to this set.
														// and will need to register with it.

		ForgedPolyRegistry shellMassDriverRegistry;		// ECBPolys which are specifically of the ECBPolyType::SHELL_MASSDRIVER will need to go here (in addition to the allPolysRegistry)
	protected:
		

		// strip maps for storing triangle strips for non-massdriver and massdriver shells; it is up to to the children of 
		// this class to use these/set them up as they see fit.
		std::unordered_map<int, CTV2Strip> typicalShellStrips;
		std::unordered_map<int, CTV2Strip> shellMassDriverStrips;


		std::map<int, TriangleMaterial> preferredMaterialLookup;	// a lookup map for specific materials that a Contour plan expects; each contour plan may look at this map
																	// for materials to use, how they look it up is up to each plan.

		// shared base class functions
		void insertPreferredMaterial(TriangleMaterial in_materialID);
		TriangleMaterial getPreferredMaterialAtIndex(int in_indexToLookup);

		// plan validity members
		bool wasRunSuccessful = true;	// a boolean to indicate whether or not the plan was succsesful;
										// what determines a plan to be successful is handled on a per-CPV2 basis.



};

#endif
