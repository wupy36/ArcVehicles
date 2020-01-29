// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcVehicleSeatConfig.h"
#include "ArcBaseVehicle.h"
#include "Seats/ArcVehicleSeat.h"
#include "ArcVehicleEngineSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "EngineMinimal.h"
#include "Engine.h"



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

void UArcVehicleSeatConfig::AttachPlayerToSeat(APlayerState* Player)
{
	PlayerInSeat = Player;

	if (IsValid(PlayerSeatComponent))
	{
		if (APawn* PlayerPawn = Cast<APawn>(PlayerSeatComponent->GetOwner()))
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

	BP_AttachPlayerToSeat(Player);
}

void UArcVehicleSeatConfig::UnAttachPlayerFromSeat(APlayerState* Player)
{
	BP_UnAttachPlayerFromSeat(Player);
	PlayerInSeat = nullptr;	
}

AArcVehiclePawn* UArcVehicleSeatConfig::GetSeatPawn() const
{
	if (IsDriverSeat())
	{
		return GetVehicleOwner();
	}

	return nullptr;
}

bool UArcVehicleSeatConfig::IsDriverSeat() const
{
	return GetVehicleOwner()->DriverSeatConfig == this;
}

void UArcVehicleSeatConfig_PlayerAttachment::AttachPlayerToSeat(APlayerState* Player)
{
	Super::AttachPlayerToSeat(Player);
}

void UArcVehicleSeatConfig_SeatPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UArcVehicleSeatConfig_SeatPawn, SeatPawn);
}

void UArcVehicleSeatConfig_SeatPawn::OnRep_SeatPawn(AArcVehiclePawn* OldSeatPawn)
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

	//Don't spawn the seat if we are the driver seat
	if (IsDriverSeat())
	{
		return;
	}

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

	AArcVehicleSeat* NewSeatPawn = OwnerVehicle->GetWorld()->SpawnActorDeferred<AArcVehicleSeat>(SeatPawnClass, TForm, OwnerVehicle, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(NewSeatPawn))
	{
		NewSeatPawn->SeatConfig = this;

		UGameplayStatics::FinishSpawningActor(NewSeatPawn, TForm);
	}
	else
	{
		return;
	}
	

	SeatPawn = NewSeatPawn;
	if (IsValid(SeatPawn))
	{		
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

AArcVehiclePawn* UArcVehicleSeatConfig_SeatPawn::GetSeatPawn() const
{
	AArcVehiclePawn* Pawn = Super::GetSeatPawn();
	if (IsValid(SeatPawn))
	{
		return Pawn;
	}

	return SeatPawn;
}
