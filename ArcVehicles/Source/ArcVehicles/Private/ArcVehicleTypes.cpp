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

