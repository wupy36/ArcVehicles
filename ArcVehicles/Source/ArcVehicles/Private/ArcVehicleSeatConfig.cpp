// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSeatConfig.h"



UArcVehicleSeatConfig::UArcVehicleSeatConfig()
{
	
}

bool UArcVehicleSeatConfig::IsOpenSeat() const
{
	return IsValid(PlayerInSeat);
}

class AArcBaseVehicle* UArcVehicleSeatConfig::GetVehicleOwner() const
{
	return Cast<AArcBaseVehicle>(GetOuter());
}

void UArcVehicleSeatConfig::SetupSeatAttachment_Implementation()
{

}

void UArcVehicleSeatConfig::AttachPlayerToSeat_Implementation(APlayerState* Player)
{
	PlayerInSeat = Player;
}
