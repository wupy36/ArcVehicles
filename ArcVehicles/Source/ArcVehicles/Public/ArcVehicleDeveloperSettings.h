// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ArcVehicleDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig)
class ARCVEHICLES_API UArcVehicleDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UArcVehicleDeveloperSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle Config", config)
	TSubclassOf<class UArcVehiclePlayerSeatComponent> PlayerSeatComponentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle Config", config)
	TSubclassOf<class UArcVehiclePlayerStateComponent> PlayerStateComponentClass;
};
