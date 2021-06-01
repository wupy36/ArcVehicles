// Copyright 2017-2020 Puny Human, All Rights Reserved.

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
		TWeakObjectPtr<UPrimitiveComponent> ObjA;
		TWeakObjectPtr<UPrimitiveComponent> ObjB;
	};

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	

private:

	TArray<FIgnorePair> IgnoreComponents;
public:
	const TArray<FIgnorePair>& GetIgnoreComponents() { return IgnoreComponents; };

	bool IgnoreBetween(UPrimitiveComponent* ObjA, UPrimitiveComponent* ObjB);
	bool RemoveIgnoreBetween(UPrimitiveComponent* ObjA, UPrimitiveComponent* ObjB);

	bool HasIgnoreBetween(UPrimitiveComponent* ObjA, UPrimitiveComponent* ObjB);
};
