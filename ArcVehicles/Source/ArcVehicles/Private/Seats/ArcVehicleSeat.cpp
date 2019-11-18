// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSeat.h"

// Sets default values
AArcVehicleSeat::AArcVehicleSeat()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AArcVehicleSeat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArcVehicleSeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AArcVehicleSeat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UArcVehicleSeatConfig* AArcVehicleSeat::GetSeatConfig()
{
	return SeatConfig;
}

