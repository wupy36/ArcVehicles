// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcVehiclePawn.h"
#include "ArcBaseVehicle.generated.h"

class UArcVehicleSeatConfig;
class APlayerState;

USTRUCT()
struct ARCVEHICLES_API FArcVehicleSeatChangeEvent
{
	GENERATED_USTRUCT_BODY()
public:
	static int32 ANY_SEAT;
	static int32 NO_SEAT;

	int32 FromSeat;
	int32 ToSeat;

	bool bFindEmptySeatOnFail;

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

	virtual bool CanProcessSeatChange(const FArcVehicleSeatChangeEvent& SeatChange);
	
	//Returns a valid seat if found a player in that seat, or nullptr if no seat is found.
	//If you pass nullptr for Player, then it will find the first open seat, or nullptr if all seats are full
	virtual UArcVehicleSeatConfig* FindSeatContainingPlayer(APlayerState* Player);

public:

	//Seat Configuration for the driver.  This object is always valid and must exist for the vehicle to be driveable
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", Instanced)
	UArcVehicleSeatConfig* DriverSeatConfig;

	//Additional Seat Configurations for this vehicle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Instanced)
	TArray<UArcVehicleSeatConfig*> AdditionalSeatConfigs;



private:

	void ProcessSeatChangeQueue();


	TArray<FArcVehicleSeatChangeEvent> SeatChangeQueue;

};
