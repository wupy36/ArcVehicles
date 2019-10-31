// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ArcVehicleSeatConfig.generated.h"


/**
 * 
 */
UCLASS(EditInlineNew, Abstract, Blueprintable, BlueprintType)
class ARCVEHICLES_API UArcVehicleSeatConfig : public UObject
{
	GENERATED_BODY()
public:

	UArcVehicleSeatConfig();

	UPROPERTY(EditDefaultsOnly, Category = "Attach", meta = (UseComponentPicker = true,AllowAnyActor=true, AllowedClasses="StaticMeshComponent,SkeletalMeshComponent"))
		FComponentReference AttachComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Attach")
		FName AttachSocketName;
	
	virtual void PostInitProperties() override;

};
