// Copyright 2017-2020 Puny Human, All Rights Reserved.


#include "Movement/ArcVehicleTurretMovementComp.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

UArcVehicleTurretMovementComp::UArcVehicleTurretMovementComp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIgnorePitch = false;
	bIgnoreYaw = false;
	bIgnoreRoll = true;

	OldBaseQuat = FQuat::Identity;
	RotationRate = FRotator(360.0f, 360.0f, 0.0f);
	CurrentBase = nullptr;

	PostPhysicsTickFunction.bCanEverTick = true;
	PostPhysicsTickFunction.bStartWithTickEnabled = true;
	PostPhysicsTickFunction.SetTickFunctionEnable(true);
	PostPhysicsTickFunction.TickGroup = TG_PostPhysics;
}

void UArcVehicleTurretMovementComp::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(PawnOwner) || !IsValid(UpdatedComponent))
	{
		return;
	}

	CheckForUpdatedBase();


	if (const AController* Controller = PawnOwner->GetController())
	{
		const bool bLocallyControlled = PawnOwner->IsLocallyControlled() && GetNetMode() == NM_Client;		
		
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
		FRotator DeltaRotation = GetDeltaRotation(DeltaTime);

		FRotator DesiredRotation = CurrentRotation;

		if (IsValid(PawnOwner->Controller))
		{
			DesiredRotation = PawnOwner->Controller->GetControlRotation();
		}
		else
		{
			return;
		}

		DesiredRotation = NormalizeOrIgnoreAxis(DesiredRotation);

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
		{
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRotation.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRotation.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRotation.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("UArcVehicleTurretMovementComp::TickComponent(): DesiredRotation"));

			PerformRotationMove(DesiredRotation);		
			
			if (GetOwnerRole() == ROLE_SimulatedProxy)
			{
				Server_ServerMove(DesiredRotation);
			}
		}		
	}
}

void UArcVehicleTurretMovementComp::PerformRotationMove(FRotator DesiredRotation)
{
	FRotator ComponentRotation = DesiredRotation;

	if (IsValid(UpdatedPitchComponent))
	{
		ComponentRotation.Pitch = 0;
	}

	MoveUpdatedComponent(FVector::ZeroVector, ComponentRotation, /*bSweep*/ false);

	if (IsValid(UpdatedPitchComponent))
	{
		FRotator PitchCompPitch(DesiredRotation.Pitch, 0, 0);
		UpdatedPitchComponent->SetRelativeRotationExact(PitchCompPitch, false);
	}
}

void UArcVehicleTurretMovementComp::PostPhysicsTickComponent(float DeltaTime, FArcVehicleTurretMovementPostPhysicsTickFunction& ThisTickFunction)
{
	UpdateBasedMovement(DeltaTime);
}

USceneComponent* UArcVehicleTurretMovementComp::GetParentAttachedBase()
{
	if (!IsValid(PawnOwner))
	{
		return nullptr;
	}
	if (!IsValid(GetPawnOwner()->GetRootComponent()))
	{
		return nullptr;
	}
	return PawnOwner->GetRootComponent()->GetAttachParent();
}

void UArcVehicleTurretMovementComp::UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation)
{
	AController* Controller = PawnOwner ? PawnOwner->Controller : NULL;
	float ControllerRoll = 0.f;
	if (Controller && !bIgnoreBaseRotation)
	{
		FRotator const ControllerRot = Controller->GetControlRotation();
		ControllerRoll = ControllerRot.Roll;
		Controller->SetControlRotation(ControllerRot + ReducedRotation);
	}

	// Remove roll
	FinalRotation.Roll = 0.f;
	if (Controller)
	{
		FinalRotation.Roll = UpdatedComponent->GetComponentRotation().Roll;
		FRotator NewRotation = Controller->GetControlRotation();
		NewRotation.Roll = ControllerRoll;
		Controller->SetControlRotation(NewRotation);
	}
}
namespace ArcVehicleMovement
{
	float GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime)
	{
		return (InAxisRotationRate >= 0.f) ? (InAxisRotationRate * DeltaTime) : 360.f;
	}
}


FRotator UArcVehicleTurretMovementComp::GetDeltaRotation(float DeltaTime) const
{
	return FRotator(ArcVehicleMovement::GetAxisDeltaRotation(RotationRate.Pitch, DeltaTime), ArcVehicleMovement::GetAxisDeltaRotation(RotationRate.Yaw, DeltaTime), ArcVehicleMovement::GetAxisDeltaRotation(RotationRate.Roll, DeltaTime));
}

FRotator UArcVehicleTurretMovementComp::NormalizeOrIgnoreAxis(FRotator DesiredRotation) const
{
	DesiredRotation.Pitch = bIgnorePitch ? 0 : FRotator::NormalizeAxis(DesiredRotation.Pitch);
	DesiredRotation.Yaw = bIgnoreYaw ? 0 : FRotator::NormalizeAxis(DesiredRotation.Yaw);
	DesiredRotation.Roll = bIgnoreRoll ? 0 : FRotator::NormalizeAxis(DesiredRotation.Roll);

	return DesiredRotation;
}

namespace ArcVehicleMovement
{
	void AddTickDependency(FTickFunction& BasedObjectTick, USceneComponent* NewBase)
	{
		if (NewBase)
		{
			if (NewBase->PrimaryComponentTick.bCanEverTick)
			{
				BasedObjectTick.AddPrerequisite(NewBase, NewBase->PrimaryComponentTick);
			}

			AActor* NewBaseOwner = NewBase->GetOwner();
			if (NewBaseOwner)
			{
				if (NewBaseOwner->PrimaryActorTick.bCanEverTick)
				{
					BasedObjectTick.AddPrerequisite(NewBaseOwner, NewBaseOwner->PrimaryActorTick);
				}

				// @TODO: We need to find a more efficient way of finding all ticking components in an actor.
				for (UActorComponent* Component : NewBaseOwner->GetComponents())
				{
					// Dont allow a based component (e.g. a particle system) to push us into a different tick group
					if (Component && Component->PrimaryComponentTick.bCanEverTick && Component->PrimaryComponentTick.TickGroup <= BasedObjectTick.TickGroup)
					{
						BasedObjectTick.AddPrerequisite(Component, Component->PrimaryComponentTick);
					}
				}
			}
		}
	}

	void RemoveTickDependency(FTickFunction& BasedObjectTick, USceneComponent* OldBase)
	{
		if (OldBase)
		{
			BasedObjectTick.RemovePrerequisite(OldBase, OldBase->PrimaryComponentTick);
			AActor* OldBaseOwner = OldBase->GetOwner();
			if (OldBaseOwner)
			{
				BasedObjectTick.RemovePrerequisite(OldBaseOwner, OldBaseOwner->PrimaryActorTick);

				// @TODO: We need to find a more efficient way of finding all ticking components in an actor.
				for (UActorComponent* Component : OldBaseOwner->GetComponents())
				{
					if (Component && Component->PrimaryComponentTick.bCanEverTick)
					{
						BasedObjectTick.RemovePrerequisite(Component, Component->PrimaryComponentTick);
					}
				}
			}
		}
	}
}

void UArcVehicleTurretMovementComp::CheckForUpdatedBase()
{
	if (CurrentBase != GetParentAttachedBase())
	{
		ArcVehicleMovement::RemoveTickDependency(PrimaryComponentTick, CurrentBase);

		CurrentBase = GetParentAttachedBase();
		ArcVehicleMovement::AddTickDependency(PrimaryComponentTick, CurrentBase);
	}
}

void UArcVehicleTurretMovementComp::UpdateBasedMovement(float DeltaTime)
{
	FQuat DeltaQuat = FQuat::Identity;

	if (USceneComponent* BaseComponent = GetParentAttachedBase())
	{
		FQuat NewBaseQuat = BaseComponent->GetComponentQuat();
		FVector NewBaseLocation = BaseComponent->GetComponentLocation();

		const bool bRotationChanged = !OldBaseQuat.Equals(NewBaseQuat, 1e-8f);
		const bool bLocationChanged = OldBaseLocation != NewBaseLocation;

		if (bRotationChanged && !bIgnoreBaseRotation)
		{
			DeltaQuat = NewBaseQuat * OldBaseQuat.Inverse();
			if (bLocationChanged)
			{
				FQuat FinalQuat = UpdatedComponent->GetComponentQuat();

				const FQuat PawnOldQuat = UpdatedComponent->GetComponentQuat();
				const FQuat TargetQuat = DeltaQuat * FinalQuat;

				FRotator TargetRot(TargetQuat);
				PawnOwner->FaceRotation(TargetRot, 0.0f);

				FinalQuat = UpdatedComponent->GetComponentQuat();

				if (PawnOldQuat.Equals(FinalQuat, 1e-6f))
				{
					MoveUpdatedComponent(FVector::ZeroVector, TargetRot, false);
					FinalQuat = UpdatedComponent->GetComponentQuat();
				}

				if (IsValid(PawnOwner->Controller))
				{
					const FQuat PawnDeltaRotation = FinalQuat * PawnOldQuat.Inverse();
					FRotator FinalRotation = FinalQuat.Rotator();
					UpdateBasedRotation(FinalRotation, PawnDeltaRotation.Rotator());
					FinalQuat = UpdatedComponent->GetComponentQuat();
				}
			}
		}

		OldBaseQuat = BaseComponent->GetComponentQuat();
		OldBaseLocation = BaseComponent->GetComponentLocation();
	}
}



void UArcVehicleTurretMovementComp::Server_ServerMove_Implementation(FRotator FullRotation)
{
	PerformRotationMove(FullRotation);
}

bool UArcVehicleTurretMovementComp::Server_ServerMove_Validate(FRotator FullRotation)
{
	return true;
}

void FArcVehicleTurretMovementPostPhysicsTickFunction::ExecuteTick(float DeltaTime, enum ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	FActorComponentTickFunction::ExecuteTickHelper(Target, /*bTickInEditor=*/ false, DeltaTime, TickType, [this](float DilatedTime)
		{
			Target->PostPhysicsTickComponent(DilatedTime, *this);
		});
}

FString FArcVehicleTurretMovementPostPhysicsTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[FArcVehicleTurretMovementPostPhysicsTickFunction::PostPhysicsTick]");
}

FName FArcVehicleTurretMovementPostPhysicsTickFunction::DiagnosticContext(bool bDetailed)
{
	if (bDetailed)
	{
		return FName(*FString::Printf(TEXT("ArcVehicleTurretMovementPostPhysicsTick/%s"), *GetFullNameSafe(Target)));
	}

	return FName(TEXT("ArcVehicleTurretMovementPostPhysicsTick"));
}
