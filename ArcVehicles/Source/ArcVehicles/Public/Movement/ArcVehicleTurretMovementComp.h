// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ArcVehicleTurretMovementComp.generated.h"

USTRUCT()
struct FArcVehicleTurretMovementPostPhysicsTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

		class UArcVehicleTurretMovementComp* Target;

	/**
	* Abstract function actually execute the tick.
	* @param DeltaTime - frame time to advance, in seconds
	* @param TickType - kind of tick for this frame
	* @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
	* @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completion of this task until certain child tasks are complete.
	**/
	virtual void ExecuteTick(float DeltaTime, enum ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;

	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
	virtual FString DiagnosticMessage() override;
	/** Function used to describe this tick for active tick reporting. **/
	virtual FName DiagnosticContext(bool bDetailed) override;
};

template<>
struct TStructOpsTypeTraits<FArcVehicleTurretMovementPostPhysicsTickFunction> : public TStructOpsTypeTraitsBase2<FArcVehicleTurretMovementPostPhysicsTickFunction>
{
	enum
	{
		WithCopy = false
	};
};

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

	/** Tick function called after physics (sync scene) has finished simulation, before cloth */
	virtual void PostPhysicsTickComponent(float DeltaTime, FArcVehicleTurretMovementPostPhysicsTickFunction& ThisTickFunction);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreBaseRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Turret Movement (Rotation Settings)")
	bool bIgnorePitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreYaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Movement (Rotation Settings)")
	bool bIgnoreRoll;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Movement")
	USceneComponent* UpdatedPitchComponent;

	/** Change in rotation per second. Set a negative value for infinite rotation rate and instant turns. */
	UPROPERTY(Category = "Turret Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	FRotator RotationRate;

	USceneComponent* GetParentAttachedBase();

	void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation);

	virtual FRotator GetDeltaRotation(float DeltaTime) const;

	virtual FRotator NormalizeOrIgnoreAxis(FRotator DesiredRotation) const;

	void CheckForUpdatedBase();
	void UpdateBasedMovement(float DeltaTime);

	virtual void PerformRotationMove(FRotator DesiredRotation);

		

protected:

	FQuat OldBaseQuat;
	FVector OldBaseLocation;

	UPROPERTY()
	USceneComponent* CurrentBase;

private:
	/** Post-physics tick function for this character */
	UPROPERTY()
		struct FArcVehicleTurretMovementPostPhysicsTickFunction PostPhysicsTickFunction;

public:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ServerMove(FRotator FullRotation);
	void Server_ServerMove_Implementation(FRotator FullRotation);
	bool Server_ServerMove_Validate(FRotator FullRotation);
};
