// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleBPFunctionLibrary.h"

AArcBaseVehicle* UArcVehicleBPFunctionLibrary::GetVehicleFromSeatConfig(FArcVehicleSeatReference SeatRef)
{
	return SeatRef.Vehicle;
}

UArcVehicleSeatConfig* UArcVehicleBPFunctionLibrary::GetVehicleSeatConfigFromRef(FArcVehicleSeatReference SeatRef)
{
	return *SeatRef;
}

bool UArcVehicleBPFunctionLibrary::IsSeatRefValid(FArcVehicleSeatReference SeatRef)
{
	return SeatRef.IsValid();
}
