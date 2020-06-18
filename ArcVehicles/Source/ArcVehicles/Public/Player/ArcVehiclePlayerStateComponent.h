// Copyright 2017-2020 Puny Human, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcVehiclePlayerStateComponent.generated.h"


UCLASS( ClassGroup=(ArcVehicles), meta=(BlueprintSpawnableComponent), Blueprintable )
class ARCVEHICLES_API UArcVehiclePlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArcVehiclePlayerStateComponent();

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Vehicle")
	APawn* StoredPlayerPawn;

};
