// 2017-2020 Puny Human.  All Rights Reserved

#include "ArcVehicles.h"
#include "ArcVehicleTypes.h"
#include "ArcBaseVehicle.h"
#include "ArcVehicleSeatConfig.h"

class USceneComponent* FArcOwnerAttachmentReference::GetSceneComponent(AActor* OwnerActor)
{
	TInlineComponentArray<USceneComponent*> Components;
	OwnerActor->GetComponents(Components);

	for (USceneComponent* Comp : Components)
	{
		if (Comp->GetFName() == ComponentName)
		{
			return Comp;
		}
	}

	return nullptr;
}

FArcVehicleScopedRelativeTransformRestoration::FArcVehicleScopedRelativeTransformRestoration(AActor* InActor)
{
	if (IsValid(InActor))
	{
		TInlineComponentArray<USceneComponent*> Components(InActor);
		for (USceneComponent* Comp : Components)
		{
			if (Comp == InActor->GetRootComponent())
			{
				continue;
			}

			ComponentTransformMap.Add(Comp, Comp->GetRelativeTransform());
		}
	}
}

FArcVehicleScopedRelativeTransformRestoration::FArcVehicleScopedRelativeTransformRestoration()
{

}

FArcVehicleScopedRelativeTransformRestoration::~FArcVehicleScopedRelativeTransformRestoration()
{
	Restore();
}

void FArcVehicleScopedRelativeTransformRestoration::Restore()
{
	for (const auto& KVP : ComponentTransformMap)
	{
		USceneComponent* Comp = KVP.Key.Get();
		const FTransform& Transform = KVP.Value;

		if (IsValid(Comp))
		{
			Comp->SetRelativeTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
}

FArcVehicleSeatReference::FArcVehicleSeatReference(UArcVehicleSeatConfig* SeatConfig)
	: Vehicle(nullptr)
	, SeatID(INDEX_NONE)
{
	Vehicle = SeatConfig->GetVehicleOwner();
	if (::IsValid(Vehicle))
	{
		SeatID = Vehicle->GetSeatIndex(SeatConfig);
	}
	
}

UArcVehicleSeatConfig* FArcVehicleSeatReference::operator->()
{
	check(IsValid());
	UArcVehicleSeatConfig* Seat = Vehicle->GetSeatConfig(*this);
	check(::IsValid(Seat));

	return Seat;
}

UArcVehicleSeatConfig* FArcVehicleSeatReference::operator*()
{
	if (!IsValid())
	{
		return nullptr;
	}
	return Vehicle->GetSeatConfig(*this);
}

bool FArcVehicleSeatReference::IsValid() const
{
	return ::IsValid(Vehicle) && SeatID >= 0;
}

bool FArcVehicleSeatReference::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Vehicle;
	Ar << SeatID;
	bOutSuccess = true;
	return true;
}

FString FArcVehicleSeatReference::ToString() const
{
	return FString::Printf(TEXT("SeatRef %s(%d)"), ::IsValid(Vehicle) ? *Vehicle->GetName() : TEXT("null"), SeatID);
}
