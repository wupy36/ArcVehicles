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

	bool bIgnoreAnyRestrictions;

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

	virtual void PostInitProperties() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Restart();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UArcVehicleSeatConfig* GetSeatConfig() override;
	virtual UArcVehicleSeatConfig* GetDriverSeat();

	virtual AArcBaseVehicle* GetOwningVehicle() override;
	

	virtual void SetupVehicleSeats();

	UFUNCTION(BlueprintNativeEvent)
	void SetupSeat(UArcVehicleSeatConfig* SeatConfig);
	virtual void SetupSeat_Implementation(UArcVehicleSeatConfig* SeatConfig);

	virtual void GetAllSeats(TArray<UArcVehicleSeatConfig*>& Seats);

	virtual bool CanProcessSeatChange(const FArcVehicleSeatChangeEvent& SeatChange);
	
	//Returns a valid seat if found a player in that seat, or nullptr if no seat is found.
	//If you pass nullptr for Player, then it will find the first open seat, or nullptr if all seats are full
	virtual UArcVehicleSeatConfig* FindSeatContainingPlayer(APlayerState* Player);

	UFUNCTION(BlueprintCallable, Category = "ArcVehicles|Vehicle")
	virtual void RequestEnterAnySeat(APlayerState* InPlayerState);

	UFUNCTION(BlueprintCallable, Category = "ArcVehicles|Vehicle")
	virtual void RequestLeaveVehicle(APlayerState* InPlayerState);

	UFUNCTION(BlueprintCallable, Category = "ArcVehicles|Vehicle")
	virtual void RequestEnterSeat(APlayerState* InPlayerState, int32 RequestedSeatIndex, bool bIgnoreRestrictions = false);

	UFUNCTION(BlueprintPure, Category = "ArcVehicles|Vehicle")
	bool IsValidSeatIndex(int32 InSeatIndex) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ArcVehicles|Vehicle")
	FTransform GetNearestExitTransform(FTransform InputLocation);
	virtual FTransform GetNearestExitTransform_Implementation(FTransform InputLocation);

	UFUNCTION(BlueprintPure, Category = "ArcVehicles|Vehicle")
	virtual void GetSortedExitPoints(FTransform InputLocation, TArray<FTransform>& OutTransformArray) const;

	virtual void PushSeatChangeEvent(const FArcVehicleSeatChangeEvent& SeatChangeEvent);
	virtual void GetAllVehicleActors(TArray<AActor*>& VehicleActors);

	virtual void NotifyPlayerSeatChangeEvent_Implementation(APlayerState* Player, UArcVehicleSeatConfig* ToSeat, UArcVehicleSeatConfig* FromSeat, EArcVehicleSeatChangeType SeatChangeEvent);

public:

	//Seat Configuration for the driver.  This object is always valid and must exist for the vehicle to be driveable
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", Instanced)
	UArcVehicleSeatConfig* DriverSeatConfig;

	//Additional Seat Configurations for this vehicle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", Instanced)
	TArray<UArcVehicleSeatConfig*> AdditionalSeatConfigs;

	UPROPERTY(Replicated, Transient)
	TArray<UArcVehicleSeatConfig*> ReplicatedSeatConfigs;

private:

	void ProcessSeatChangeQueue();

	UPROPERTY()
	TArray<FArcVehicleSeatChangeEvent> SeatChangeQueue;

};
