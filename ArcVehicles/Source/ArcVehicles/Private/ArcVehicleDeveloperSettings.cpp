// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleDeveloperSettings.h"
#include "Player/ArcVehiclePlayerSeatComponent.h"
#include "Player/ArcVehiclePlayerStateComponent.h"

UArcVehicleDeveloperSettings::UArcVehicleDeveloperSettings()
	: Super()
{
	PlayerSeatComponentClass = UArcVehiclePlayerSeatComponent::StaticClass();
	PlayerStateComponentClass = UArcVehiclePlayerStateComponent::StaticClass();
}
