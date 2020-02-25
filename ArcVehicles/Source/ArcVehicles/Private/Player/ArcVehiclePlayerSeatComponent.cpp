// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcBaseVehicle.h"
#include "Player/ArcVehiclePlayerSeatComponent.h"
#include "ArcVehicleSeatConfig.h"
#include "ArcVehicleEngineSubsystem.h"
#include "Components/PrimitiveComponent.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"


// Sets default values for this component's properties
UArcVehiclePlayerSeatComponent::UArcVehiclePlayerSeatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);


	// ...
}


void UArcVehiclePlayerSeatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UArcVehiclePlayerSeatComponent, SeatConfig, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(UArcVehiclePlayerSeatComponent, StoredPlayerState);
}

// Called when the game starts
void UArcVehiclePlayerSeatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UArcVehiclePlayerSeatComponent::OnRegister()
{
	Super::OnRegister();

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
		{
			StoredPlayerState = OwnerPawn->GetPlayerState();
		}
	}
}

void UArcVehiclePlayerSeatComponent::ChangeSeats(UArcVehicleSeatConfig* NewSeat)
{

	PreviousSeatConfig = SeatConfig;
	SeatConfig = NewSeat;

	EArcVehicleSeatChangeType SeatChangeType = EArcVehicleSeatChangeType::Invalid;

	//We've entered a vehicle
	if (PreviousSeatConfig == nullptr && IsValid(SeatConfig))
	{
		SeatChangeType = EArcVehicleSeatChangeType::EnterVehicle;
		
	}
	//We've swapped seats
	if (IsValid(PreviousSeatConfig) && IsValid(SeatConfig))
	{
		SeatChangeType = EArcVehicleSeatChangeType::SwitchSeats;
	}
	//We've exited the vehicle
	if (IsValid(PreviousSeatConfig) && SeatConfig == nullptr)
	{
		SeatChangeType = EArcVehicleSeatChangeType::ExitVehicle;
	}
	//There is no 4th case (when both seats are nullptr.  That shouldn't happen.  If it does... ignore it.

	OnSeatChangeEvent(SeatChangeType);

	//Inform the vehicle of this seat change event on both client and server
	
	AArcBaseVehicle* Vehicle = nullptr;
	if (IsValid(SeatConfig))
	{
		Vehicle = SeatConfig->GetVehicleOwner();
	}
	else if (IsValid(PreviousSeatConfig))
	{
		Vehicle = PreviousSeatConfig->GetVehicleOwner();
	}

	if (IsValid(Vehicle))
	{
		Vehicle->NotifyPlayerSeatChangeEvent(StoredPlayerState, SeatConfig, PreviousSeatConfig, SeatChangeType);
	}
}

void UArcVehiclePlayerSeatComponent::OnRep_SeatConfig(UArcVehicleSeatConfig* InPreviousSeatConfig)
{
	//So, we reverted the replication here because ChangeSeats stores the previous seat and changes SeatConfig itself.
	UArcVehicleSeatConfig* CurrentSeat = SeatConfig;
	SeatConfig = InPreviousSeatConfig;

	//Make sure that on the client, we know the seat is ours.  This isn't replicated from the server but we can derive it so bandwidth savings.
	if (IsValid(CurrentSeat))
	{
		CurrentSeat->PlayerSeatComponent = this;
	}

	ChangeSeats(CurrentSeat);
}

void UArcVehiclePlayerSeatComponent::OnSeatChangeEvent_Implementation(EArcVehicleSeatChangeType SeatChangeType)
{
	if (IsValid(PreviousSeatConfig))
	{
		PreviousSeatConfig->UnAttachPlayerFromSeat(StoredPlayerState);
	}

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (SeatChangeType == EArcVehicleSeatChangeType::EnterVehicle || SeatChangeType == EArcVehicleSeatChangeType::SwitchSeats)
		{
			if (IsValid(SeatConfig))
			{
				UArcVehicleEngineSubsystem* EngSub = GEngine->GetEngineSubsystem<UArcVehicleEngineSubsystem>();
				TInlineComponentArray<UPrimitiveComponent*> VehicleComponents(SeatConfig->GetVehicleOwner());
				TInlineComponentArray<UPrimitiveComponent*> OwnerPawnComponents(OwnerPawn);

				for (UPrimitiveComponent* VC : VehicleComponents)
				{
					for (UPrimitiveComponent* SC : OwnerPawnComponents)
					{
						EngSub->IgnoreBetween(VC, SC);
					}
				}

 				if (GetOwnerRole() == ROLE_Authority)
				{
					SeatConfig->AttachPlayerToSeat(StoredPlayerState);

					if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
					{
						OwnerChar->GetCharacterMovement()->StopMovementImmediately();
						OwnerChar->GetCharacterMovement()->DisableMovement();
						OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(false);
					}
				}				
			}
		}

		if (SeatChangeType == EArcVehicleSeatChangeType::ExitVehicle)
		{
			if (GetOwnerRole() == ROLE_Authority)
			{
				//Reset the player.  If they are invisible, make them visible
				OwnerPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				OwnerPawn->SetActorHiddenInGame(false);


				FVector ExitLoc = GetOwner()->GetActorLocation() + FVector(0, 0, 300);
				if (IsValid(PreviousSeatConfig))
				{
					TArray<FTransform> ExitLocations;
					PreviousSeatConfig->GetVehicleOwner()->GetSortedExitPoints(GetOwner()->GetActorTransform(), ExitLocations);

					//TODO: Trace to this exit point
					if (ExitLocations.Num() > 0)
					{
						FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(OwnerPawn->GetSimpleCollisionCylinderExtent());
						
						const ECollisionChannel CollisionChannel = OwnerPawn->GetRootComponent()->GetCollisionObjectType();

						FCollisionQueryParams QueryParams;
						TArray<AActor*> Actors;
						PreviousSeatConfig->GetVehicleOwner()->GetAllVehicleActors(Actors);
						OwnerPawn->GetAttachedActors(Actors, false);
						Actors.Add(OwnerPawn);

						QueryParams.AddIgnoredActors(Actors);

						for (const FTransform& TestLocation : ExitLocations)
						{
							FHitResult Hit;
							bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, OwnerPawn->GetActorLocation(), TestLocation.GetLocation(), ECC_Pawn, QueryParams);
							if (!bBlockingHit)
							{
								ExitLoc = TestLocation.GetLocation();
								break;
							}
						}
					}
				}
				FHitResult HitResult;
				OwnerPawn->SetActorLocationAndRotation(ExitLoc, FQuat::Identity, true, &HitResult, ETeleportType::ResetPhysics);

				if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
				{
					OwnerChar->GetCharacterMovement()->StopMovementImmediately();
					OwnerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(true);
				}
			}		

			UArcVehicleEngineSubsystem* EngSub = GEngine->GetEngineSubsystem<UArcVehicleEngineSubsystem>();
			TInlineComponentArray<UPrimitiveComponent*> VehicleComponents(PreviousSeatConfig->GetVehicleOwner());
			TInlineComponentArray<UPrimitiveComponent*> OwnerPawnComponents(OwnerPawn);

			for (UPrimitiveComponent* VC : VehicleComponents)
			{
				for (UPrimitiveComponent* SC : OwnerPawnComponents)
				{
					EngSub->RemoveIgnoreBetween(VC, SC);
				}
			}			
		}
	}
	
}

