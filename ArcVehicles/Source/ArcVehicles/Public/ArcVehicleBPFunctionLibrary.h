// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArcVehicleTypes.h"
#include "ArcVehicleBPFunctionLibrary.generated.h"

class AArcBaseVehicle;

/**
 * 
 */
UCLASS()
class ARCVEHICLES_API UArcVehicleBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="ArcVehicles")
	static AArcBaseVehicle* GetVehicleFromSeatConfig(FArcVehicleSeatReference SeatRef);

	UFUNCTION(BlueprintPure, Category = "ArcVehicles")
	static UArcVehicleSeatConfig* GetVehicleSeatConfigFromRef(FArcVehicleSeatReference SeatRef);

	UFUNCTION(BlueprintPure, Category = "ArcVehicles")
	static bool IsSeatRefValid(FArcVehicleSeatReference SeatRef);
};
