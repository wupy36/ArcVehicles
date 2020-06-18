// Copyright 2017-2020 Puny Human, All Rights Reserved.

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
