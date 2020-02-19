// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ArcVehicleTurretMovementComp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (ArcVehicles), meta = (BlueprintSpawnableComponent), Blueprintable)
class ARCVEHICLES_API UArcVehicleTurretMovementComp : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	
	UArcVehicleTurretMovementComp(const FObjectInitializer& ObjectInitializer);

	//Begin UActorComponent Interface
	/** Applies rotation to UpdatedComponent. */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreBaseRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Turret Movement (Rotation Settings)")
	bool bIgnorePitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreYaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreRoll;

	/** Change in rotation per second. Set a negative value for infinite rotation rate and instant turns. */
	UPROPERTY(Category = "Turret Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	FRotator RotationRate;

	USceneComponent* GetParentAttachedBase();

	void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation);

	virtual FRotator GetDeltaRotation(float DeltaTime) const;

	virtual FRotator NormalizeOrIgnoreAxis(FRotator DesiredRotation) const;

	void CheckForUpdatedBase();

protected:

	FQuat OldBaseQuat;

	UPROPERTY()
	USceneComponent* CurrentBase;
};
