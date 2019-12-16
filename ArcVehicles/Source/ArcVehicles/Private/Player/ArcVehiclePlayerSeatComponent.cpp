// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcVehiclePlayerSeatComponent.h"
#include "EngineMinimal.h"


// Sets default values for this component's properties
UArcVehiclePlayerSeatComponent::UArcVehiclePlayerSeatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UArcVehiclePlayerSeatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UArcVehiclePlayerSeatComponent, SeatConfig, COND_None, REPNOTIFY_Always);
}

// Called when the game starts
void UArcVehiclePlayerSeatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UArcVehiclePlayerSeatComponent::ChangeSeats(UArcVehicleSeatConfig* NewSeat)
{

	PreviousSeatConfig = SeatConfig;
	SeatConfig = NewSeat;

	//We've entered a vehicle
	if (PreviousSeatConfig == nullptr && IsValid(SeatConfig))
	{
		OnSeatChangeEvent(EArcVehicleSeatChangeType::EnterVehicle);
	}
	//We've swapped seats
	if (IsValid(PreviousSeatConfig) && IsValid(SeatConfig))
	{
		OnSeatChangeEvent(EArcVehicleSeatChangeType::SwitchSeats);
	}
	//We've exited the vehicle
	if (IsValid(PreviousSeatConfig) && SeatConfig == nullptr)
	{
		OnSeatChangeEvent(EArcVehicleSeatChangeType::ExitVehicle);
	}
	//There is no 4th case (when both seats are nullptr.  That shouldn't happen.  If it does... ignore it.
}

void UArcVehiclePlayerSeatComponent::OnRep_SeatConfig(UArcVehicleSeatConfig* InPreviousSeatConfig)
{
	//So, we reverted the replication here because ChangeSeats stores the previous seat and changes SeatConfig itself.
	UArcVehicleSeatConfig* CurrentSeat = SeatConfig;
	SeatConfig = InPreviousSeatConfig;
	ChangeSeats(CurrentSeat);
}

void UArcVehiclePlayerSeatComponent::OnSeatChangeEvent_Implementation(EArcVehicleSeatChangeType SeatChangeType)
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (SeatChangeType == EArcVehicleSeatChangeType::EnterVehicle || SeatChangeType == EArcVehicleSeatChangeType::SwitchSeats)
		{
			if (IsValid(SeatConfig))
			{
				TInlineComponentArray<UPrimitiveComponent*, 10> PrimComps(OwnerPawn);
				for (UPrimitiveComponent* comp : PrimComps)
				{
					if (!PreviousVehicleCollisionResponses.Contains(comp))
					{
						PreviousVehicleCollisionResponses.Add(comp, comp->GetCollisionResponseToChannel(ECC_Vehicle));
					}					  
					comp->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
				}

				SeatConfig->AttachPlayerToSeat(OwnerPawn->GetPlayerState());

				if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
				{
					OwnerChar->GetCharacterMovement()->StopMovementImmediately();
					OwnerChar->GetCharacterMovement()->DisableMovement();
					OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(false);
				}
			}
		}

		if (SeatChangeType == EArcVehicleSeatChangeType::ExitVehicle)
		{
			//Reset the player.  If they are invisible, make them visible
			OwnerPawn->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			OwnerPawn->SetActorHiddenInGame(false);
			

			FVector ExitLoc = GetOwner()->GetActorLocation() + FVector(0, 0, 300);
			if (IsValid(PreviousSeatConfig))
			{
				ExitLoc = PreviousSeatConfig->GetVehicleOwner()->GetActorLocation() + FVector(0, 0, 300);
			}
			

			OwnerPawn->SetActorLocationAndRotation(ExitLoc, FQuat::Identity, false);

			TInlineComponentArray<UPrimitiveComponent*, 10> PrimComps(OwnerPawn);
			for (UPrimitiveComponent* comp : PrimComps)
			{
				if (PreviousVehicleCollisionResponses.Contains(comp))
				{
					comp->SetCollisionResponseToChannel(ECC_Vehicle, PreviousVehicleCollisionResponses[comp]);
				}
				
			}

			if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
			{
				OwnerChar->GetCharacterMovement()->StopMovementImmediately();
				OwnerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(true);
			}
		}

	}
	
}

