// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcBaseVehicle.h"
#include "ArcVehicleSeatConfig.h"

FArcVehicleSeatChangeEvent::NO_SEAT = INDEX_NONE;
FArcVehicleSeatChangeEvent::ANY_SEAT = INT32_MAX;


// Sets default values
AArcBaseVehicle::AArcBaseVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void AArcBaseVehicle::SetupVehicleSeats()
{
	TArray<UArcVehicleSeatConfig*> Seats;
	GetAllSeats(Seats);

	for (UArcVehicleSeatConfig* SeatConfig : Seats)
	{
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

