// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ArcVehicleExitPoint.generated.h"


UCLASS( ClassGroup=(ArcVehicles), meta=(BlueprintSpawnableComponent) )
class ARCVEHICLES_API UArcVehicleExitPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArcVehicleExitPoint();
		
};
