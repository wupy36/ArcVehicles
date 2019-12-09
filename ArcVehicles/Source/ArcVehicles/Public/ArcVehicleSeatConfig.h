// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ArcVehicleTypes.h"
#include "ArcVehicleSeatConfig.generated.h"

class UArcVehiclePlayerSeatComponent;

/**
 * 
 */
UCLASS(EditInlineNew, Abstract, Blueprintable, BlueprintType)
class ARCVEHICLES_API UArcVehicleSeatConfig : public UObject
{
	GENERATED_BODY()
public:

	UArcVehicleSeatConfig();

	//Where the seat is attached to on the parent vehicle
	UPROPERTY(EditDefaultsOnly, Category = "Attach")
	FArcOwnerAttachmentReference AttachSeatToComponent;

	UPROPERTY(VisibleInstanceOnly, Category="Seat")
	APlayerState* PlayerInSeat;

	UPROPERTY()
	UArcVehiclePlayerSeatComponent* PlayerSeatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Character")
	bool bPlayerVisible;


	UFUNCTION(BlueprintPure)
	virtual bool IsOpenSeat() const;

	UFUNCTION(BlueprintPure)
	class AArcBaseVehicle* GetVehicleOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetupSeatAttachment();
	virtual void SetupSeatAttachment_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AttachPlayerToSeat(APlayerState* Player);
	virtual void AttachPlayerToSeat_Implementation(APlayerState* Player);
};

UCLASS()
class ARCVEHICLES_API UArcVehicleSeatConfig_PlayerAttachment : public UArcVehicleSeatConfig
{
	GENERATED_BODY()
public:
	
	virtual void AttachPlayerToSeat_Implementation(APlayerState* Player) override;
	//TODO: Animation Stuff


	
};


UCLASS()
class ARCVEHICLES_API UArcVehicleSeatConfig_SeatPawn : public UArcVehicleSeatConfig_PlayerAttachment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Seat Pawn")
	TSubclassOf<class AArcVehicleSeat> SeatPawnClass;

	/** Property to point to the template child actor for details panel purposes */
	//UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Seat Pawn", meta = (ShowInnerProperties))
	//AArcVehicleSeat* SeatPawnTemplate;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Character")
	FArcOwnerAttachmentReference PlayerCharacterAttachToComponent;
};
