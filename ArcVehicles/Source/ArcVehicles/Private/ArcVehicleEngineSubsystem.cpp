// Copyright 2017-2020 Puny Human, All Rights Reserved.


#include "ArcVehicleEngineSubsystem.h"
#include "Physics/PhysicsFiltering.h"
#include "PhysicsEngine/RigidBodyIndexPair.h"
#include "Physics/PhysicsInterfaceDeclares.h"





void UArcVehicleEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{


}

bool UArcVehicleEngineSubsystem::IgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB)
{
	if (ObjA == ObjB)
	{
		return false;
	}
	if (HasIgnoreBetween(ObjA, ObjB))
	{
		return true;
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

bool UArcVehicleEngineSubsystem::HasIgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB)
{
	for (int i = 0; i < IgnoreComponents.Num(); i++)
	{
		FIgnorePair& IP = IgnoreComponents[i];

		if ((IP.ObjA == ObjA || IP.ObjA == ObjB)
			&& (IP.ObjB == ObjA || IP.ObjB == ObjB))
		{
			return true;
		}
	}

	return false;
}
