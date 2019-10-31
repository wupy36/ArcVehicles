// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ArcVehicleSeatConfig.generated.h"

USTRUCT(BlueprintType)
struct FArcSeatAttachConfig
{
	GENERATED_USTRUCT_BODY()
public:


	UPROPERTY(BlueprintReadWrite, Category = "Attach")
	FName AttachComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Attach")
	FName AttachSocketName;

};

/**
 * 
 */
UCLASS(EditInlineNew, Abstract, Blueprintable, BlueprintType)
class ARCVEHICLES_API UArcVehicleSeatConfig : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SeatPawn")
	FArcSeatAttachConfig SeatPawnAttachConfig;


};
