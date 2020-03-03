// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcVehicleTypes.h"
#include "ArcVehiclePlayerSeatComponent.generated.h"

class UArcVehicleSeatConfig;
class APlayerState;



UCLASS(ClassGroup = (ArcVehicles), meta = (BlueprintSpawnableComponent), Blueprintable)
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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Vehicle", ReplicatedUsing = OnRep_SeatConfig)
		UArcVehicleSeatConfig* SeatConfig;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Vehicle")
		UArcVehicleSeatConfig* PreviousSeatConfig;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Vehicle", Replicated)
		APlayerState* StoredPlayerState;

	FArcVehicleScopedRelativeTransformRestoration RelativeTransformRestorer;

	UFUNCTION()
		virtual void OnRep_SeatConfig(UArcVehicleSeatConfig* InPreviousSeatConfig);

	UFUNCTION(BlueprintNativeEvent)
		void OnSeatChangeEvent(EArcVehicleSeatChangeType SeatChangeType);
	void OnSeatChangeEvent_Implementation(EArcVehicleSeatChangeType SeatChangeType);



	static void OnShowDebugInfo(class AHUD* HUD, class UCanvas* Canvas, const class FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);


	/**
	* Draw important variables on canvas.
	*
	* @param Canvas - Canvas to draw on
	* @param DebugDisplay - Contains information about what debug data to display
	* @param YL - Height of the current font
	* @param YPos - Y position on Canvas. YPos += YL, gives position to draw text for next debug line.
	*/
	virtual void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos);

	virtual void GenerateDebugStrings(TArray<FString>& OutStrings);

	/** Ask the server to send ability system debug information back to the client, via ClientPrintDebug_Response  */
	UFUNCTION(Server, reliable, WithValidation)
		void ServerPrintDebug_Request();
	void ServerPrintDebug_Request_Implementation();
	bool ServerPrintDebug_Request_Validate();

	UFUNCTION(Client, reliable)
		void ClientPrintDebug_Response(const TArray<FString>& Strings);
	void ClientPrintDebug_Response_Implementation(const TArray<FString>& Strings);
	bool ClientPrintDebug_Response_Validate(const TArray<FString>& Strings);

	virtual bool ShouldRequestDebugStrings() const;

	TArray<FString> ServerDebugStrings;

	EArcVehicleSeatChangeType DebugLastSeatChangeType;

protected:
	UPROPERTY()
		TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionResponse>> PreviousVehicleCollisionResponses;
};
