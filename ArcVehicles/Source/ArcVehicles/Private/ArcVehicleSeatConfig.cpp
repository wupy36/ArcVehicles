// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSeatConfig.h"
#include "EngineMinimal.h"



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

	if (IsValid(Player))
	{
		if (APawn* PlayerPawn = Player->GetPawn())
		{		

			USceneComponent* SceneComponent = AttachSeatToComponent.GetSceneComponent(GetVehicleOwner());
			if (IsValid(SceneComponent))
			{				
				PlayerPawn->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSeatToComponent.SocketName);		
				PlayerPawn->SetActorHiddenInGame(!bPlayerVisible);				
			}
			else
			{
				PlayerPawn->AttachToActor(GetVehicleOwner(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				PlayerPawn->SetActorHiddenInGame(true); //If we are always attaching to the actor, just hide us
			}
			
		}
	}
	
}

void UArcVehicleSeatConfig_PlayerAttachment::AttachPlayerToSeat_Implementation(APlayerState* Player)
{
	Super::AttachPlayerToSeat_Implementation(Player);
}
