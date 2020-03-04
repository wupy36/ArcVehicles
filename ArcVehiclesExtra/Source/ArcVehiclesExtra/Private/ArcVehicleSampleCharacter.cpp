// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSampleCharacter.h"
#include "ArcBaseVehicle.h"

// Sets default values
AArcVehicleSampleCharacter::AArcVehicleSampleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseAttachedVehicleRelevancy = true;
}

// Called when the game starts or when spawned
void AArcVehicleSampleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArcVehicleSampleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AArcVehicleSampleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AArcVehicleSampleCharacter::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation)  const
{
	if (bUseAttachedVehicleRelevancy)
	{
		if (AArcBaseVehicle* Vehicle = Cast<AArcBaseVehicle>(GetAttachParentActor()))
		{
			return Vehicle->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
		}
	}
	
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

