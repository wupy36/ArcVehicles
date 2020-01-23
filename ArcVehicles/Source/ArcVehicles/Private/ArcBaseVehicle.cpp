// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcVehicles.h"
#include "ArcBaseVehicle.h"
#include "EngineMinimal.h"
#include "ArcVehicleSeatConfig.h"
#include "GameFramework/PlayerState.h"
#include "Player/ArcVehiclePlayerSeatComponent.h"
#include "Player/ArcVehiclePlayerStateComponent.h"
#include "ArcVehicleDeveloperSettings.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

int32 FArcVehicleSeatChangeEvent::NO_SEAT = INDEX_NONE;
int32 FArcVehicleSeatChangeEvent::ANY_SEAT = INT32_MAX;


// Sets default values
AArcBaseVehicle::AArcBaseVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AArcBaseVehicle::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool AArcBaseVehicle::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool bWroteSomething = false;

	TArray<UArcVehicleSeatConfig*> AllSeats;
	GetAllSeats(AllSeats);

	for (UArcVehicleSeatConfig* SeatConfig : AllSeats)
	{
		bWroteSomething |= Channel->ReplicateSubobject(SeatConfig, *Bunch, *RepFlags);		
	}

	return bWroteSomething;
}

// Called when the game starts or when spawned
void AArcBaseVehicle::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetupVehicleSeats();
	}
	
}

// Called every frame
void AArcBaseVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		ProcessSeatChangeQueue();
	}

}

// Called to bind functionality to input
void AArcBaseVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UArcVehicleSeatConfig* AArcBaseVehicle::GetSeatConfig()
{
	return DriverSeatConfig;
}

AArcBaseVehicle* AArcBaseVehicle::GetOwningVehicle() 
{
	return this;
}

void AArcBaseVehicle::SetupVehicleSeats()
{
	TArray<UArcVehicleSeatConfig*> Seats;
	GetAllSeats(Seats);

	for (UArcVehicleSeatConfig* SeatConfig : Seats)
	{
		if (SeatConfig == DriverSeatConfig)
		{
			if (UArcVehicleSeatConfig_SeatPawn* PawnConfig = Cast<UArcVehicleSeatConfig_SeatPawn>(SeatConfig))
			{
				PawnConfig->SeatPawnClass = nullptr;
				PawnConfig->SeatPawn = this;
			}
		}

		SetupSeat(SeatConfig);
	}
}

void AArcBaseVehicle::SetupSeat_Implementation(UArcVehicleSeatConfig* SeatConfig)
{
	if (IsValid(SeatConfig))
	{
		SeatConfig->SetupSeatAttachment();
	}
}

void AArcBaseVehicle::GetAllSeats(TArray<UArcVehicleSeatConfig*>& Seats)
{
	Seats.Reset(AdditionalSeatConfigs.Num() + 1);

	Seats.Add(DriverSeatConfig);
	Seats.Append(AdditionalSeatConfigs);
}

bool AArcBaseVehicle::CanProcessSeatChange(const FArcVehicleSeatChangeEvent& SeatChange)
{
	return true;
}

UArcVehicleSeatConfig* AArcBaseVehicle::FindSeatContainingPlayer(APlayerState* Player)
{
	TArray<UArcVehicleSeatConfig*> AllSeats;
	GetAllSeats(AllSeats);

	for (UArcVehicleSeatConfig* Config : AllSeats)
	{
		if (Config->PlayerInSeat == Player)
		{
			return Config;
		}
	}

	return nullptr;
}

void AArcBaseVehicle::RequestEnterAnySeat(APlayerState* InPlayerState)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	FArcVehicleSeatChangeEvent SeatChangeEvent;
	SeatChangeEvent.Player = InPlayerState;
	SeatChangeEvent.FromSeat = FArcVehicleSeatChangeEvent::NO_SEAT;
	SeatChangeEvent.ToSeat = FArcVehicleSeatChangeEvent::ANY_SEAT;
	SeatChangeEvent.bFindEmptySeatOnFail = true;

	PushSeatChangeEvent(SeatChangeEvent);
}

void AArcBaseVehicle::RequestLeaveVehicle(APlayerState* InPlayerState)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	FArcVehicleSeatChangeEvent SeatChangeEvent;
	SeatChangeEvent.Player = InPlayerState;
	SeatChangeEvent.FromSeat = FArcVehicleSeatChangeEvent::ANY_SEAT;
	SeatChangeEvent.ToSeat = FArcVehicleSeatChangeEvent::NO_SEAT;

	PushSeatChangeEvent(SeatChangeEvent);
}

void AArcBaseVehicle::RequestEnterSeat(APlayerState* InPlayerState, int32 RequestedSeat, bool bIgnoreRestrictions /*= false*/)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	//If we don't have a valid seat, return
	if (!IsValidSeatIndex(RequestedSeat))
	{
		return;
	}
	

	FArcVehicleSeatChangeEvent SeatChangeEvent;
	SeatChangeEvent.Player = InPlayerState;
	SeatChangeEvent.FromSeat = FArcVehicleSeatChangeEvent::ANY_SEAT;
	SeatChangeEvent.ToSeat = RequestedSeat;
	SeatChangeEvent.bIgnoreAnyRestrictions = bIgnoreRestrictions;

	PushSeatChangeEvent(SeatChangeEvent);
}

bool AArcBaseVehicle::IsValidSeatIndex(int32 InSeatIndex) const
{
	if (InSeatIndex < 0)
	{
		return false;
	}

	if (InSeatIndex >= AdditionalSeatConfigs.Num() + 1)
	{
		return false;
	}

	return true;
}

void AArcBaseVehicle::PushSeatChangeEvent(const FArcVehicleSeatChangeEvent& SeatChangeEvent)
{
	if (CanProcessSeatChange(SeatChangeEvent))
	{
		SeatChangeQueue.Push(SeatChangeEvent);
	}	
}

void AArcBaseVehicle::ProcessSeatChangeQueue()
{
	TArray<UArcVehicleSeatConfig*> AllSeats;
	GetAllSeats(AllSeats);

	while (SeatChangeQueue.Num() > 0)
	{
		FArcVehicleSeatChangeEvent SeatChangeEvent = SeatChangeQueue.Pop(false);
		
		//If we don't have a player, then we have a problem.
		//We can safely just ignore in shipping, but we should let the developer
		//know something went wrong
		if (!ensure(IsValid(SeatChangeEvent.Player)))
		{
			continue;
		}

		//PROCESSING SEAT CHANGES
		//Seat Changes are processed in a sequential way.  Firstly, we find the actual seats that the player wants to go to.
		//The seats can be nullptr.  There are fail conditions if seats are nullptr and there is some intent
		//Then we determine intent (eg: FromSeat ANY, ToSeat NOSEAT means the player just wants out of the vehicle)
		//This helps us process the change.  There are two possible actions that can be taken based on the player's intent
		//  First Action is "PUT PLAYER INTO SEAT".  This takes a player (regardless of where they are) and places them directly in the seat.  
		//  Second is "REMOVE PLAYER FROM VEHICLE".  This takes a player (regardless of where they are) and removes them from the seat that they are in.
		//If a player is moving from one seat to another, we take the first action and then clear out their FromSeat reference pointer.  
		

		if (CanProcessSeatChange(SeatChangeEvent))
		{
			//Get the seat objects from this seat change event.  
			//The Event contains Intents, and ToSeat and FromSeat are the actual possible situations.
			UArcVehicleSeatConfig* ToSeat = nullptr;
			UArcVehicleSeatConfig* FromSeat = nullptr;
			if (SeatChangeEvent.FromSeat >= 0)
			{
				//Find the seat that the player is coming from
				if (AllSeats.IsValidIndex(SeatChangeEvent.FromSeat))
				{
					FromSeat = AllSeats[SeatChangeEvent.FromSeat];

					check(IsValid(FromSeat)); //Seat Must be valid (bad data?)
					
					//If the player isn't the player in the seat, we have a bad data situation
					checkf(FromSeat->PlayerInSeat == SeatChangeEvent.Player, 
						TEXT("AArcBaseVehicle::ProcessSeatChangeQueue: Got a Seat Change Event where a different player is in FromSeat.  Player changing seats: %s Player In FromSeat: %s"),
					*SeatChangeEvent.Player->GetPlayerName(), FromSeat->PlayerInSeat ? *FromSeat->PlayerInSeat->GetPlayerName() : TEXT("NULLPTR"));
				}
				else if (SeatChangeEvent.FromSeat == FArcVehicleSeatChangeEvent::ANY_SEAT)
				{
					//If we are coming from any seat, that doesn't make a whole lot of sense.  So we are going to assume the caller doesn't know
					// where the player is.  So, let's find them.  If we don't find them, then we assume that it's the same as they aren't in the vehicle 
					// and want to get in.

					//Finding a player this way is kind of slow.  The caller should know that. 
					FromSeat = FindSeatContainingPlayer(SeatChangeEvent.Player);
				}
			}
			
			if (SeatChangeEvent.ToSeat >= 0)
			{
				if (AllSeats.IsValidIndex(SeatChangeEvent.ToSeat))
				{
					ToSeat = AllSeats[SeatChangeEvent.ToSeat];
					check(IsValid(ToSeat)); //Seat must be valid (bad data?)

					//If IsValid(ToSeat->PlayerInSeat), that means someone is in this seat
					if (IsValid(ToSeat->PlayerInSeat) && SeatChangeEvent.bFindEmptySeatOnFail)
					{
						ToSeat = FindSeatContainingPlayer(nullptr);
					}
					else if (IsValid(ToSeat->PlayerInSeat))
					{
						//This is a fail to do something situation.  
						//We need to discover the intent of this change event, then notify the player that it failed.
						ToSeat = nullptr;
					}
				}
				else if (SeatChangeEvent.ToSeat == FArcVehicleSeatChangeEvent::ANY_SEAT)
				{
					ToSeat = FindSeatContainingPlayer(nullptr);
				}
			}



			//PLAYER INFORMATION
			//Player information is stored in two objects, the PlayerStateComponent (attached to a PlayerState) and the PlayerSeat Component.
			//Player State Component stores information for the player state.  What actor the player was previously possessing before changing it for vehicles, etc. It is authority-only
			//Player Seat Component is replicated and is for any pawn that is attached to the vehicle

			//Check if this player has a PlayerStateComp.  If not, create one for them
			UArcVehiclePlayerStateComponent* PlayerStateComp = SeatChangeEvent.Player->FindComponentByClass<UArcVehiclePlayerStateComponent>();
			if (!IsValid(PlayerStateComp))
			{
				const UArcVehicleDeveloperSettings* Settings = GetDefault<UArcVehicleDeveloperSettings>();
				TSubclassOf<UArcVehiclePlayerStateComponent> StateCompClass = Settings->PlayerStateComponentClass;

				PlayerStateComp = NewObject<UArcVehiclePlayerStateComponent>(SeatChangeEvent.Player, StateCompClass);
				PlayerStateComp->RegisterComponent();
			}

			check(IsValid(PlayerStateComp));

			APawn* PlayerPawn = SeatChangeEvent.Player->GetPawn();

			//If the pawn is this or it's owner is this, then the player is sitting in the vehicle and possessing one of our pawns
			//So use our old pawn
			if (PlayerPawn == this || PlayerPawn->GetOwner() == this)
			{
				PlayerPawn = PlayerStateComp->StoredPlayerPawn;
			}

			check(IsValid(PlayerPawn));

			UArcVehiclePlayerSeatComponent* PlayerSeatComponent = PlayerPawn->FindComponentByClass<UArcVehiclePlayerSeatComponent>();
			if (!IsValid(PlayerSeatComponent))
			{
				const UArcVehicleDeveloperSettings* Settings = GetDefault<UArcVehicleDeveloperSettings>();
				TSubclassOf<UArcVehiclePlayerSeatComponent> StateCompClass = Settings->PlayerSeatComponentClass;

				PlayerSeatComponent = NewObject<UArcVehiclePlayerSeatComponent>(PlayerPawn);
				PlayerSeatComponent->RegisterComponent();
			}

			check(IsValid(PlayerSeatComponent));


			//Now to derive the Intent from the event, and what we can actually do
			const bool bWantsOut = SeatChangeEvent.ToSeat == FArcVehicleSeatChangeEvent::NO_SEAT;
			const bool bWantsASeat = SeatChangeEvent.ToSeat >= 0;
			
			if (bWantsASeat)
			{
				if (IsValid(ToSeat))
				{
					if (IsValid(FromSeat))
					{
						//remove the player from this seat.  
						FromSeat->PlayerInSeat = nullptr;
					}

					//Put the player into the seat		
					PlayerSeatComponent->ChangeSeats(ToSeat);
					ToSeat->PlayerInSeat = SeatChangeEvent.Player;
					ToSeat->PlayerSeatComponent = PlayerSeatComponent;

					//a seat with a seat pawn (like the driver seat), take the controller from the player state and posses us.
					AArcVehiclePawn* SeatPawn = ToSeat->GetSeatPawn();
					if (IsValid(SeatPawn))
					{
						SeatPawn->BecomePossessedByPlayer(SeatChangeEvent.Player);
						PlayerStateComp->StoredPlayerPawn = PlayerPawn;
					}
					else
					{
						//Make sure the player possesses their own pawn
						if (AController* SeatedPlayerController = Cast<AController>(SeatChangeEvent.Player->GetOwner()))
						{
							if (SeatedPlayerController->GetPawn() != PlayerPawn)
							{
								SeatedPlayerController->Possess(PlayerStateComp->StoredPlayerPawn);
								PlayerStateComp->StoredPlayerPawn = nullptr;
							}							
						}
					}
				}
				else
				{
					//FAIL CONDITION.  Player wants a seat, but there is no seat for them (or the seat is taken)
					//TODO: Inform the player.

					continue;
				}
			}

			if (bWantsOut)
			{
				if (IsValid(FromSeat))
				{		
					PlayerSeatComponent->ChangeSeats(nullptr);
					FromSeat->PlayerInSeat = nullptr;

					if (AController* SeatedPlayerController = Cast<AController>(SeatChangeEvent.Player->GetOwner()))
					{
						SeatedPlayerController->Possess(PlayerStateComp->StoredPlayerPawn);
						PlayerStateComp->StoredPlayerPawn = nullptr;
					}
					
				}
				else
				{
					//FAIL CONDITION.  The player isn't in the vehicle.  For now, we can just continue
					continue;
				}
			}			
			 
		}
	}
}

