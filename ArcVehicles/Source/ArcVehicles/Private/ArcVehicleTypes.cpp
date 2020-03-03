// 2017-2020 Puny Human.  All Rights Reserved

#include "ArcVehicles.h"
#include "ArcVehicleTypes.h"

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

FArcVehicleScopedRelativeTransformRestoration::~FArcVehicleScopedRelativeTransformRestoration()
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
