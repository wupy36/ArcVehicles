// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcVehicleSeatConfig.h"
#include "ArcBaseVehicle.h"
#include "Seats/ArcVehicleSeat.h"
#include "ArcVehicleEngineSubsystem.h"
#include "Net/UnrealNetwork.h"
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

void UArcVehicleSeatConfig_SeatPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArcVehicleSeatConfig_SeatPawn, SeatPawn);
}

void UArcVehicleSeatConfig_SeatPawn::OnRep_SeatPawn(AArcVehicleSeat* OldSeatPawn)
{
	if (!IsValid(SeatPawn))
	{
		return;
	}

	AArcBaseVehicle* OwnerVehicle = GetVehicleOwner();
	check(IsValid(OwnerVehicle));


	UArcVehicleEngineSubsystem* EngSub = GEngine->GetEngineSubsystem<UArcVehicleEngineSubsystem>();
	TInlineComponentArray<UPrimitiveComponent*> VehicleComponents(OwnerVehicle);
	TInlineComponentArray<UPrimitiveComponent*> SeatPawnComponents(SeatPawn);

	for (UPrimitiveComponent* VC : VehicleComponents)
	{
		for (UPrimitiveComponent* SC : SeatPawnComponents)
		{
			EngSub->IgnoreBetween(VC, SC);
		}
	}
}

void UArcVehicleSeatConfig_SeatPawn::SetupSeatAttachment_Implementation()
{
	Super::SetupSeatAttachment_Implementation();

	if (!ensure(IsValid(SeatPawnClass)))
	{
		return;
	}

	AArcBaseVehicle* OwnerVehicle = GetVehicleOwner();
	check(IsValid(OwnerVehicle));	

	USceneComponent* SC = AttachSeatToComponent.GetSceneComponent(OwnerVehicle);
	FTransform TForm = FTransform::Identity;
	if (IsValid(SC))
	{
		TForm = SC->GetSocketTransform(AttachSeatToComponent.SocketName, RTS_World);
	}
	else
	{
		TForm = OwnerVehicle->GetActorTransform();
	}

	SeatPawn = OwnerVehicle->GetWorld()->SpawnActorDeferred<AArcVehicleSeat>(SeatPawnClass, TForm, OwnerVehicle, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);	

	UGameplayStatics::FinishSpawningActor(SeatPawn, TForm);

	if (IsValid(SeatPawn))
	{
		SeatPawn->SeatConfig = this;
		if (IsValid(SC))
		{

			SeatPawn->AttachToComponent(SC, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSeatToComponent.SocketName);
		}
		else
		{
			SeatPawn->AttachToActor(OwnerVehicle, FAttachmentTransformRules::SnapToTargetIncludingScale);
		}
	}

	OnRep_SeatPawn(nullptr);
}
