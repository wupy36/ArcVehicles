// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSeatConfig.h"



UArcVehicleSeatConfig::UArcVehicleSeatConfig()
{
	
}

class AArcBaseVehicle* UArcVehicleSeatConfig::GetVehicleOwner() const
{
	return Cast<AArcBaseVehicle>(GetOuter());
}

void UArcVehicleSeatConfig::SetupSeatAttachment_Implementation()
{

}

void UArcVehicleSeatConfig_PlayerAttachment::AttachPlayerToSeat_Implementation(AActor* PlayerActor)
{

}
