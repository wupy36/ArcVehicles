// Copyright 2017-2020 Puny Human, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
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
