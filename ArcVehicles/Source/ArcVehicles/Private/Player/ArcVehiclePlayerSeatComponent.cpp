// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcBaseVehicle.h"
#include "Player/ArcVehiclePlayerSeatComponent.h"
#include "ArcVehicleSeatConfig.h"
#include "ArcVehicleEngineSubsystem.h"
#include "Components/PrimitiveComponent.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"

#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "DisplayDebugHelpers.h"


// Sets default values for this component's properties
UArcVehiclePlayerSeatComponent::UArcVehiclePlayerSeatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	RelativeTransformRestorer = FArcVehicleScopedRelativeTransformRestoration(nullptr);
	// ...
}


void UArcVehiclePlayerSeatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UArcVehiclePlayerSeatComponent, SeatConfig, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArcVehiclePlayerSeatComponent, StoredPlayerState, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArcVehiclePlayerSeatComponent, ServerDebugStrings, COND_None, REPNOTIFY_Always)
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

	RelativeTransformRestorer = FArcVehicleScopedRelativeTransformRestoration(GetOwner());
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

	DebugLastSeatChangeType = SeatChangeType;
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
		CurrentSeat->PlayerInSeat = StoredPlayerState;
	}

	ChangeSeats(CurrentSeat);

	//Clean out the player seat component on the client.  The seat change code handles this on the server
	if (IsValid(InPreviousSeatConfig))
	{
		InPreviousSeatConfig->PlayerInSeat = nullptr;
		InPreviousSeatConfig->PlayerSeatComponent = nullptr;
	}
}

void UArcVehiclePlayerSeatComponent::OnRep_StoredPlayerState(APlayerState* InPreviousPlayerState)
{
	BP_OnRep_StoredPlayerState(InPreviousPlayerState);
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
				SetIgnoreBetween(SeatConfig->GetVehicleOwner());				

				{
					SeatConfig->AttachPlayerToSeat(StoredPlayerState);
				}

				if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
				{
					if (GetOwnerRole() == ROLE_Authority)
					{
						OwnerChar->GetCharacterMovement()->FlushServerMoves();
						OwnerChar->ForceNetUpdate();
						OwnerChar->GetCharacterMovement()->ForceReplicationUpdate();
						OwnerChar->GetCharacterMovement()->ForceClientAdjustment();
					}
					OwnerChar->GetCharacterMovement()->StopMovementImmediately();
					OwnerChar->GetCharacterMovement()->DisableMovement();
					OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(false);

				}
			}
		}

		if (SeatChangeType == EArcVehicleSeatChangeType::ExitVehicle)
		{
			//Enable player movement
			if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerPawn))
			{
				OwnerChar->GetCharacterMovement()->StopMovementImmediately();
				OwnerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				OwnerChar->GetCharacterMovement()->SetComponentTickEnabled(true);
				if (GetOwnerRole() == ROLE_Authority)
				{
					OwnerChar->GetCharacterMovement()->FlushServerMoves();
					OwnerChar->GetCharacterMovement()->ForceReplicationUpdate();
					OwnerChar->GetCharacterMovement()->ForceClientAdjustment();
					OwnerChar->ForceNetUpdate();
				}
			}


			//Find them an exit point.  This has to be done after we re-enable movement otherwise we don't get teleported
			if (GetOwnerRole() == ROLE_Authority)
			{
				FVector ExitLoc = GetOwner()->GetActorLocation() + FVector(0, 0, 300);
				if (IsValid(PreviousSeatConfig))
				{
					TArray<FTransform> ExitLocations;
					PreviousSeatConfig->GetVehicleOwner()->GetSortedExitPoints(GetOwner()->GetActorTransform(), ExitLocations);

					if (ExitLocations.Num() > 0)
					{		
						for (const FTransform& TestLocation : ExitLocations)
						{
							FVector TestLocVec = TestLocation.GetLocation();
							if (GetWorld()->FindTeleportSpot(GetOwner(), TestLocVec, TestLocation.GetRotation().Rotator()))
							{
								ExitLoc = TestLocVec;
								break;
							}							
						}
					}
				}

				OwnerPawn->SetActorLocationAndRotation(ExitLoc, FQuat::Identity, false, nullptr, ETeleportType::TeleportPhysics);
			}

			//Force the player to right itself
			OwnerPawn->SetActorRotation(FQuat::Identity);

			{
				//Reset the player.  If they are invisible, make them visible
				FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
				OwnerPawn->DetachFromActor(DetachmentRules);
				OwnerPawn->SetActorHiddenInGame(false);
			}

			//Allow them to collide with everything
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

	RelativeTransformRestorer.Restore();
}

void UArcVehiclePlayerSeatComponent::SetIgnoreBetween(AActor* OtherActor)
{
	UArcVehicleEngineSubsystem* EngSub = GEngine->GetEngineSubsystem<UArcVehicleEngineSubsystem>();
	TInlineComponentArray<UPrimitiveComponent*> VehicleComponents(OtherActor);
	TInlineComponentArray<UPrimitiveComponent*> OwnerPawnComponents(GetOwner());

	for (UPrimitiveComponent* VC : VehicleComponents)
	{
		for (UPrimitiveComponent* SC : OwnerPawnComponents)
		{
			EngSub->IgnoreBetween(VC, SC);
		}
	}
}

namespace ArcVehiclesDebug
{
	struct FDebugTargetInfo
	{
		FDebugTargetInfo()
		{

		}

		TWeakObjectPtr<UWorld> TargetWorld;
		TWeakObjectPtr<UArcVehiclePlayerSeatComponent> LastDebugTarget;
	};

	TArray<FDebugTargetInfo> InventoryDebugInfoList;

	FDebugTargetInfo* GetDebugTargetInfo(UWorld* World)
	{
		FDebugTargetInfo* TargetInfo = nullptr;
		for (FDebugTargetInfo& Info : InventoryDebugInfoList)
		{
			if (Info.TargetWorld.Get() == World)
			{
				TargetInfo = &Info;
				break;
			}
		}
		if (TargetInfo == nullptr)
		{
			TargetInfo = &InventoryDebugInfoList[InventoryDebugInfoList.AddDefaulted()];
			TargetInfo->TargetWorld = World;
		}

		return TargetInfo;
	}

	UArcVehiclePlayerSeatComponent* GetDebugTarget(FDebugTargetInfo* TargetInfo)
	{
		//Return the Target if we have one
		if (UArcVehiclePlayerSeatComponent* Inv = TargetInfo->LastDebugTarget.Get())
		{
			return Inv;
		}

		//Find one
		for (TObjectIterator<UArcVehiclePlayerSeatComponent> It; It; ++It)
		{
			if (UArcVehiclePlayerSeatComponent* Inv = *It)
			{
				if (Inv->GetWorld() == TargetInfo->TargetWorld.Get() && MakeWeakObjectPtr(Inv).Get())
				{
					TargetInfo->LastDebugTarget = Inv;

					//Default to local player
					if (APawn* Pawn = Cast<APawn>(Inv->GetOwner()))
					{
						if (Pawn->IsLocallyControlled())
						{
							break;
						}
					}
				}
			}
		}
		return TargetInfo->LastDebugTarget.Get();
	}

	FString PrintDebugSeatInfo(UArcVehicleSeatConfig* SeatConfig)
	{
		if (!IsValid(SeatConfig))
		{
			return TEXT("null");
		}

		return FString::Printf(TEXT("Seat (%s) Owner(%s), IsDriver(%s) PlayerInSeat (%s), PlayerCompInSeat (%s) Vehicle (%s) VehicleOwner (%s)"),
			*SeatConfig->GetClass()->GetAuthoredName(),
			*SeatConfig->GetVehicleOwner()->GetName(),
			(SeatConfig->GetVehicleOwner()->GetDriverSeat() == SeatConfig) ? TEXT("true") : TEXT("false"),
			IsValid(SeatConfig->PlayerInSeat) ? *SeatConfig->PlayerInSeat->GetPlayerName() : TEXT("null"),
			IsValid(SeatConfig->PlayerSeatComponent) ? *SeatConfig->PlayerSeatComponent->GetName() : TEXT("null"),
			IsValid(SeatConfig->GetVehicleOwner()) ? *SeatConfig->GetVehicleOwner()->GetName() : TEXT("null"),
			IsValid(SeatConfig->GetVehicleOwner()->GetOwner()) ? *SeatConfig->GetVehicleOwner()->GetOwner()->GetName() : TEXT("null")
		);
	}

	void CycleDebugTarget(FDebugTargetInfo* TargetInfo, bool Next)
	{
		GetDebugTarget(TargetInfo);

		// Build a list	of ASCs
		TArray<UArcVehiclePlayerSeatComponent*> List;
		for (TObjectIterator<UArcVehiclePlayerSeatComponent> It; It; ++It)
		{
			if (UArcVehiclePlayerSeatComponent* SeatComp = *It)
			{
				if (SeatComp->GetWorld() == TargetInfo->TargetWorld.Get())
				{
					List.Add(SeatComp);
				}
			}
		}

		if (List.Num() == 0)
		{
			return;
		}

		// Search through list to find prev/next target
		UArcVehiclePlayerSeatComponent* Previous = nullptr;
		for (int32 idx = 0; idx < List.Num() + 1; ++idx)
		{
			UArcVehiclePlayerSeatComponent* SeatComp = List[idx % List.Num()];

			if (Next && Previous == TargetInfo->LastDebugTarget.Get())
			{
				TargetInfo->LastDebugTarget = SeatComp;
				return;
			}
			if (!Next && SeatComp == TargetInfo->LastDebugTarget.Get())
			{
				TargetInfo->LastDebugTarget = Previous;
				return;
			}

			Previous = SeatComp;
		}
	}

	static void	VehicleCycleDebugTarget(UWorld* InWorld, bool Next)
	{
		CycleDebugTarget(GetDebugTargetInfo(InWorld), Next);
	}

	FAutoConsoleCommandWithWorld VehicleSeatNextDebugTargetCmd(
		TEXT("ArcVehicleSeat.Debug.NextTarget"),
		TEXT("Targets next PlayerSeat in ShowDebug VehicleSeat"),
		FConsoleCommandWithWorldDelegate::CreateStatic(VehicleCycleDebugTarget, true)
	);

	FAutoConsoleCommandWithWorld VehicleSeatPrevDebugTargetCmd(
		TEXT("ArcVehicleSeat.Debug.PrevTarget"),
		TEXT("Targets previous PlayerSeat in ShowDebug VehicleSeat"),
		FConsoleCommandWithWorldDelegate::CreateStatic(VehicleCycleDebugTarget, false)
	);
}

void UArcVehiclePlayerSeatComponent::OnShowDebugInfo(class AHUD* HUD, class UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
	if (DisplayInfo.IsDisplayOn(TEXT("VehicleSeat")))
	{
		UWorld* World = HUD->GetWorld();
		ArcVehiclesDebug::FDebugTargetInfo* TargetInfo = ArcVehiclesDebug::GetDebugTargetInfo(World);

		if (UArcVehiclePlayerSeatComponent* Comp = ArcVehiclesDebug::GetDebugTarget(TargetInfo))
		{
			TArray<FName> LocalDisplayNames;
			LocalDisplayNames.Add(TEXT("CVehicleSeat"));
			FDebugDisplayInfo LocalDisplayInfo(LocalDisplayNames, TArray<FName>());

			Comp->DisplayDebug(Canvas, LocalDisplayInfo, YL, YPos);
		}
	}
}

void UArcVehiclePlayerSeatComponent::DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	if (DebugDisplay.IsDisplayOn(TEXT("CVehicleSeat")))
	{
		FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
		TArray<FString> ClientStrings;
		GenerateDebugStrings(ClientStrings);

		DisplayDebugManager.DrawString(TEXT("---CLIENT SEAT STATE---"));
		for (const FString& String : ClientStrings)
		{
			DisplayDebugManager.DrawString(String, 15);
		}

		DisplayDebugManager.DrawString(TEXT("---SERVER SEAT STATE---"));
		for (const FString& String : ServerDebugStrings)
		{
			DisplayDebugManager.DrawString(String, 15);
		}

		//Ask for the server strings.  This is very chatty, but it's debug.
		if (GetOwnerRole() != ROLE_Authority && ShouldRequestDebugStrings())
		{
			ServerPrintDebug_Request();
		}

	}
}

void UArcVehiclePlayerSeatComponent::GenerateDebugStrings(TArray<FString>& OutStrings)
{
	OutStrings.Add(FString::Printf(TEXT("Current Seat: %s"), *ArcVehiclesDebug::PrintDebugSeatInfo(SeatConfig)));
	OutStrings.Add(FString::Printf(TEXT("Previous Seat: %s"), *ArcVehiclesDebug::PrintDebugSeatInfo(PreviousSeatConfig)));

	OutStrings.Add(FString::Printf(TEXT("Last Seat Change Event: %s"), *UEnum::GetValueAsString(DebugLastSeatChangeType)));

	OutStrings.Add(FString::Printf(TEXT("PlayerPawn: NetOwner (%s)"),
		IsValid(GetOwner()->GetOwner()) ? *GetOwner()->GetOwner()->GetName() : TEXT("null")
	));
}

void UArcVehiclePlayerSeatComponent::ServerPrintDebug_Request_Implementation()
{
	ServerDebugStrings.Empty(ServerDebugStrings.Num());
	GenerateDebugStrings(ServerDebugStrings);
}

bool UArcVehiclePlayerSeatComponent::ServerPrintDebug_Request_Validate()
{
	return true;
}

void UArcVehiclePlayerSeatComponent::OnRep_ServerDebugStrings()
{

}

bool UArcVehiclePlayerSeatComponent::ShouldRequestDebugStrings() const
{
	// This implements basic throttling so that debug strings can't be sent more than once a second to the server
	const double MinTimeBetweenClientDebugSends = 1.f;
	static double LastSendTime = 0.f;

	double CurrentTime = FPlatformTime::Seconds();
	bool ShouldSend = (CurrentTime - LastSendTime) > MinTimeBetweenClientDebugSends;
	if (ShouldSend)
	{
		LastSendTime = CurrentTime;
	}
	return ShouldSend;
}

namespace InventoryDebug
{
	FDelegateHandle DebugHandle = AHUD::OnShowDebugInfo.AddStatic(&UArcVehiclePlayerSeatComponent::OnShowDebugInfo);
}

