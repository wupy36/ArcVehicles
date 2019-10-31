// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcVehiclePawn.h"
#include "ArcBaseVehicle.generated.h"

class UArcVehicleSeatConfig;

UCLASS()
class ARCVEHICLES_API AArcBaseVehicle : public AArcVehiclePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AArcBaseVehicle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UArcVehicleSeatConfig* GetSeatConfig() override;
	
public:

	//Seat Configuration for the driver.  This object is always valid and must exist for the vehicle to be driveable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", Instanced)
	UArcVehicleSeatConfig* DriverSeatConfig;

	//Additional Seat Configurations for this vehicle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Instanced)
	TArray<UArcVehicleSeatConfig*> AdditionalSeatConfigs;

};
