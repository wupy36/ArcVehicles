// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ArcVehicleTypes.h"
#include "ArcVehicleSeatConfig.generated.h"

class UArcVehiclePlayerSeatComponent;
class AArcVehiclePawn;

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

	UFUNCTION()
	virtual void AttachPlayerToSeat(APlayerState* Player);


	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Attach Player To Seat"))
	void BP_AttachPlayerToSeat(APlayerState* Player);

	UFUNCTION()
	virtual void UnAttachPlayerFromSeat(APlayerState* Player);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Unattach Player To Seat"))
	void BP_UnAttachPlayerFromSeat(APlayerState* Player);

	virtual AArcVehiclePawn* GetSeatPawn() const;

	
	UFUNCTION(BlueprintPure)
	bool IsDriverSeat() const;
};

UCLASS()
class ARCVEHICLES_API UArcVehicleSeatConfig_PlayerAttachment : public UArcVehicleSeatConfig
{
	GENERATED_BODY()
public:
	
	virtual void AttachPlayerToSeat(APlayerState* Player) override;
	//TODO: Animation Stuff


	
};


UCLASS()
class ARCVEHICLES_API UArcVehicleSeatConfig_SeatPawn : public UArcVehicleSeatConfig_PlayerAttachment
{
	GENERATED_BODY()
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Seat Pawn")
	TSubclassOf<class AArcVehicleSeat> SeatPawnClass;

	/** Property to point to the template child actor for details panel purposes */
	//UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Seat Pawn", meta = (ShowInnerProperties))
	//AArcVehicleSeat* SeatPawnTemplate;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Character")
	FArcOwnerAttachmentReference PlayerCharacterAttachToComponent;

	UPROPERTY(ReplicatedUsing=OnRep_SeatPawn)
	AArcVehiclePawn* SeatPawn;

	UFUNCTION()
	void OnRep_SeatPawn(AArcVehiclePawn* OldSeatPawn);

	virtual void SetupSeatAttachment_Implementation() override;

	virtual AArcVehiclePawn* GetSeatPawn() const override;
};
