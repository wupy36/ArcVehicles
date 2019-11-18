// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcVehiclePawn.h"
#include "ArcBaseVehicle.generated.h"

class UArcVehicleSeatConfig;

USTRUCT()
struct ARCVEHICLES_API FArcVehicleSeatChangeEvent
{
	GENERATED_USTRUCT_BODY()
public:
	const int32 ANY_SEAT;
	const int32 NO_SEAT;

	int32 FromSeat;
	int32 ToSeat;

	bool bFindEmptySeat;

	UPROPERTY()
	APlayerState* Player;

};

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
	

	virtual void SetupVehicleSeats();

	UFUNCTION(BlueprintNativeEvent)
	void SetupSeat(UArcVehicleSeatConfig* SeatConfig);
	virtual void SetupSeat_Implementation(UArcVehicleSeatConfig* SeatConfig);

	virtual void GetAllSeats(TArray<UArcVehicleSeatConfig*>& Seats);

public:

	//Seat Configuration for the driver.  This object is always valid and must exist for the vehicle to be driveable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", Instanced)
	UArcVehicleSeatConfig* DriverSeatConfig;

	//Additional Seat Configurations for this vehicle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Instanced)
	TArray<UArcVehicleSeatConfig*> AdditionalSeatConfigs;



private:

	TArray<FArcVehicleSeatChangeEvent> SeatChangeQueue;

};
