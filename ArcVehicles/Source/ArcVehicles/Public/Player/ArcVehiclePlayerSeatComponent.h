// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcVehiclePlayerSeatComponent.generated.h"

class UArcVehicleSeatConfig;
class APlayerState;

UENUM(BlueprintType)
enum class EArcVehicleSeatChangeType : uint8
{
	EnterVehicle,
	ExitVehicle,
	SwitchSeats
};

UCLASS( ClassGroup=(ArcVehicles), meta=(BlueprintSpawnableComponent), Blueprintable )
class ARCVEHICLES_API UArcVehiclePlayerSeatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArcVehiclePlayerSeatComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void OnRegister() override;

	virtual void ChangeSeats(UArcVehicleSeatConfig* NewSeat);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Vehicle", ReplicatedUsing=OnRep_SeatConfig)
	UArcVehicleSeatConfig* SeatConfig;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Vehicle")
	UArcVehicleSeatConfig* PreviousSeatConfig;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Vehicle", Replicated)
	APlayerState* StoredPlayerState;

	UFUNCTION()
	virtual void OnRep_SeatConfig(UArcVehicleSeatConfig* InPreviousSeatConfig);

	UFUNCTION(BlueprintNativeEvent)
	void OnSeatChangeEvent(EArcVehicleSeatChangeType SeatChangeType);
	void OnSeatChangeEvent_Implementation(EArcVehicleSeatChangeType SeatChangeType);


protected:
	UPROPERTY()
	TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionResponse>> PreviousVehicleCollisionResponses;
};
