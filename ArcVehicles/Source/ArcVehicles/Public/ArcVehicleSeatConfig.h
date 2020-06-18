// Copyright 2017-2020 Puny Human, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

	//Where the seat is attached to on the parent vehicle
	UPROPERTY(EditAnywhere, Category = "Attach", Replicated)
		FArcOwnerAttachmentReference AttachSeatToComponent;

	UPROPERTY(VisibleInstanceOnly, Category = "Seat")
		APlayerState* PlayerInSeat;

	UPROPERTY()
		UArcVehiclePlayerSeatComponent* PlayerSeatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character", Replicated)
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


	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Attach Player To Seat"))
		void BP_AttachPlayerToSeat(APlayerState* Player);

	UFUNCTION()
		virtual void UnAttachPlayerFromSeat(APlayerState* Player);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Unattach Player To Seat"))
		void BP_UnAttachPlayerFromSeat(APlayerState* Player);

	virtual AArcVehiclePawn* GetSeatPawn() const;

	virtual FTransform GetSeatAttachTransform_World();
	virtual FTransform GetSawnAttachTrasnform_Relative();
	
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
	UArcVehicleSeatConfig_SeatPawn();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Seat Pawn", Replicated)
		TSubclassOf<class AArcVehicleSeat> SeatPawnClass;

	/** Property to point to the template child actor for details panel purposes */
	//UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Seat Pawn", meta = (ShowInnerProperties))
	//AArcVehicleSeat* SeatPawnTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Character", Replicated)
		FArcOwnerAttachmentReference PlayerCharacterAttachToComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Character", Replicated)
		bool bResetControlRotationOnEnter;

	UPROPERTY(ReplicatedUsing = OnRep_SeatPawn)
		AArcVehiclePawn* SeatPawn;

	UFUNCTION()
		void OnRep_SeatPawn(AArcVehiclePawn* OldSeatPawn);

	virtual void SetupSeatAttachment_Implementation() override;
	virtual void AttachPlayerToSeat(APlayerState* Player) override;

	virtual AArcVehiclePawn* GetSeatPawn() const override;
};
