// Copyright 2017-2020 Puny Human, All Rights Reserved.


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
