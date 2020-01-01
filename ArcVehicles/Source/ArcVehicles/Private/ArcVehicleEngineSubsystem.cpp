// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleEngineSubsystem.h"
#include "PhysicsFiltering.h"




#if WITH_PHYSX
PxFilterFlags PhysXSimFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

static TWeakObjectPtr<UArcVehicleEngineSubsystem> EngineSubsystem;

static PxFilterFlags PhysXSimFilterShader_VehicleAttach(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//Our code
	if (EngineSubsystem.IsValid())
	{
		const TArray<UArcVehicleEngineSubsystem::FIgnorePair>& IgnoreComponents = EngineSubsystem->GetIgnoreComponents();

		for (const UArcVehicleEngineSubsystem::FIgnorePair& Pair : IgnoreComponents)
		{
			if (Pair.ObjA.IsValid() && Pair.ObjB.IsValid())
			{
				if ((Pair.ObjA->GetUniqueID() == filterData0.word2 && Pair.ObjB->GetUniqueID() == filterData1.word2)
					|| (Pair.ObjA->GetUniqueID() == filterData1.word2 && Pair.ObjB->GetUniqueID() == filterData0.word2))
				{
					return PxFilterFlag::eSUPPRESS;
				}
			}
		}
	}

	//Call the engine version of the filter shader	
	return PhysXSimFilterShader(attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize);	
}
#endif


void UArcVehicleEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if WITH_PHYSX
	EngineSubsystem = this;	
	GSimulationFilterShader = &PhysXSimFilterShader_VehicleAttach;
#endif

}

bool UArcVehicleEngineSubsystem::IgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB)
{
	if (ObjA == ObjB)
	{
		return false;
	}

	FIgnorePair IgnorePair;
	IgnorePair.ObjA = ObjA;
	IgnorePair.ObjB = ObjB;

	ObjA->RecreatePhysicsState();
	ObjB->RecreatePhysicsState();

	return IgnoreComponents.Add(IgnorePair) >=0;
}

bool UArcVehicleEngineSubsystem::RemoveIgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB)
{
	int32 removals = 0;
	for (int i = 0; i < IgnoreComponents.Num(); i++)
	{
		FIgnorePair& IP = IgnoreComponents[i];

		if ((IP.ObjA == ObjA || IP.ObjA == ObjB)
			&& (IP.ObjB == ObjA || IP.ObjB == ObjB))
		{
			IgnoreComponents.RemoveAt(i);
			i--;
			removals++;
		}
	}

	return removals > 0;
}

//Taken from PHysXSupport line 133
#if WITH_PHYSX
PxFilterFlags PhysXSimFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//UE_LOG(LogPhysics, Log, TEXT("filterData0 (%s): %x %x %x %x"), *ObjTypeToString(attributes0), filterData0.word0, filterData0.word1, filterData0.word2, filterData0.word3);
	//UE_LOG(LogPhysics, Log, TEXT("filterData1 (%s): %x %x %x %x"), *ObjTypeToString(attributes1), filterData1.word0, filterData1.word1, filterData1.word2, filterData1.word3);

	bool k0 = PxFilterObjectIsKinematic(attributes0);
	bool k1 = PxFilterObjectIsKinematic(attributes1);

	PxU32 FilterFlags0 = (filterData0.word3 & 0xFFFFFF);
	PxU32 FilterFlags1 = (filterData1.word3 & 0xFFFFFF);

	if (k0 && k1)
	{
		//Ignore kinematic kinematic pairs unless they are explicitly requested
		if (!(FilterFlags0&EPDF_KinematicKinematicPairs) && !(FilterFlags1&EPDF_KinematicKinematicPairs))
		{
			return PxFilterFlag::eSUPPRESS;	//NOTE: Waiting on physx fix for refiltering on aggregates. For now use supress which automatically tests when changes to simulation happen
		}
	}

	bool s0 = PxGetFilterObjectType(attributes0) == PxFilterObjectType::eRIGID_STATIC;
	bool s1 = PxGetFilterObjectType(attributes1) == PxFilterObjectType::eRIGID_STATIC;

	//ignore static-kinematic (this assumes that statics can't be flagged as kinematics)
	// should return eSUPPRESS here instead eKILL so that kinematics vs statics will still be considered once kinematics become dynamic (dying ragdoll case)
	if ((k0 || k1) && (s0 || s1))
	{
		return PxFilterFlag::eSUPPRESS;
	}

	// if these bodies are from the same component, use the disable table to see if we should disable collision. This case should only happen for things like skeletalmesh and destruction. The table is only created for skeletal mesh components at the moment
#if !WITH_CHAOS
	if (filterData0.word2 == filterData1.word2)
	{
		check(constantBlockSize == sizeof(FPhysSceneShaderInfo));
		const FPhysSceneShaderInfo* PhysSceneShaderInfo = (const FPhysSceneShaderInfo*)constantBlock;
		check(PhysSceneShaderInfo);
		FPhysScene * PhysScene = PhysSceneShaderInfo->PhysScene;
		check(PhysScene);

		const TMap<uint32, TMap<FRigidBodyIndexPair, bool> *> & CollisionDisableTableLookup = PhysScene->GetCollisionDisableTableLookup();
		TMap<FRigidBodyIndexPair, bool>* const * DisableTablePtrPtr = CollisionDisableTableLookup.Find(filterData1.word2);
		if (DisableTablePtrPtr)		//Since collision table is deferred during sub-stepping it's possible that we won't get the collision disable table until the next frame
		{
			TMap<FRigidBodyIndexPair, bool>* DisableTablePtr = *DisableTablePtrPtr;
			FRigidBodyIndexPair BodyPair(filterData0.word0, filterData1.word0); // body indexes are stored in word 0
			if (DisableTablePtr->Find(BodyPair))
			{
				return PxFilterFlag::eKILL;
			}

		}
	}
#endif

	// Find out which channels the objects are in
	ECollisionChannel Channel0 = GetCollisionChannel(filterData0.word3);
	ECollisionChannel Channel1 = GetCollisionChannel(filterData1.word3);

	// see if 0/1 would like to block the other 
	PxU32 BlockFlagTo1 = (ECC_TO_BITFIELD(Channel1) & filterData0.word1);
	PxU32 BlockFlagTo0 = (ECC_TO_BITFIELD(Channel0) & filterData1.word1);

	bool bDoesWantToBlock = (BlockFlagTo1 && BlockFlagTo0);

	// if don't want to block, suppress
	if (!bDoesWantToBlock)
	{
		return PxFilterFlag::eSUPPRESS;
	}



	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//todo enabling CCD objects against everything else for now
	if (!(k0 && k1) && ((FilterFlags0&EPDF_CCD) || (FilterFlags1&EPDF_CCD)))
	{
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eSOLVE_CONTACT;
	}


	if ((FilterFlags0&EPDF_ContactNotify) || (FilterFlags1&EPDF_ContactNotify))
	{
		pairFlags |= (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_CONTACT_POINTS);
	}


	if ((FilterFlags0&EPDF_ModifyContacts) || (FilterFlags1&EPDF_ModifyContacts))
	{
		pairFlags |= (PxPairFlag::eMODIFY_CONTACTS);
	}

	return PxFilterFlags();
}
#endif