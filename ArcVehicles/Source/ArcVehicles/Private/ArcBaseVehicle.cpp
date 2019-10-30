// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcBaseVehicle.h"

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

