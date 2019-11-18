// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcVehiclePawn.h"
#include "ArcVehicleSeat.generated.h"



UCLASS(Abstract)
class ARCVEHICLES_API AArcVehicleSeat : public AArcVehiclePawn
{
	GENERATED_BODY()

public:
	friend class AArcBaseVehicle;

	// Sets default values for this pawn's properties
	AArcVehicleSeat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UArcVehicleSeatConfig* GetSeatConfig() override;

protected:

	UPROPERTY()
	UArcVehicleSeatConfig* SeatConfig;

};
