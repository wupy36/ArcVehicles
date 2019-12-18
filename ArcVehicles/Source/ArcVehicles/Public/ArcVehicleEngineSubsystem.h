// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ArcVehicleEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ARCVEHICLES_API UArcVehicleEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:

	struct FIgnorePair
	{
		TWeakObjectPtr<USceneComponent> ObjA;
		TWeakObjectPtr<USceneComponent> ObjB;
	};

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	

private:

	TArray<FIgnorePair> IgnoreComponents;
public:
	const TArray<FIgnorePair>& GetIgnoreComponents() { return IgnoreComponents; };

	bool IgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB);

	bool RemoveIgnoreBetween(USceneComponent* ObjA, USceneComponent* ObjB);
};
