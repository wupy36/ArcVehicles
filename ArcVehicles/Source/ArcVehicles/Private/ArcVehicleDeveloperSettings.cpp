// Copyright 2017-2020 Puny Human, All Rights Reserved.


#include "ArcVehicleDeveloperSettings.h"
#include "Player/ArcVehiclePlayerSeatComponent.h"
#include "Player/ArcVehiclePlayerStateComponent.h"

UArcVehicleDeveloperSettings::UArcVehicleDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerSeatComponentClass = UArcVehiclePlayerSeatComponent::StaticClass();
	PlayerStateComponentClass = UArcVehiclePlayerStateComponent::StaticClass();
}
