// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcVehiclePlayerStateComponent.generated.h"


UCLASS( ClassGroup=(ArcVehicles), meta=(BlueprintSpawnableComponent) )
class ARCVEHICLES_API UArcVehiclePlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArcVehiclePlayerStateComponent();

	UPROPERTY()
	APawn* StoredPlayerPawn;

};
