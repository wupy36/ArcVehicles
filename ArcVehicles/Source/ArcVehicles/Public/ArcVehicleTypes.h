// 2017-2020 Puny Human.  All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ArcVehicleTypes.generated.h"

USTRUCT(BlueprintType)
struct ARCVEHICLES_API FArcOwnerAttachmentReference
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach")
		FName ComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attach")
		FName SocketName;

	class USceneComponent* GetSceneComponent(AActor* OwnerActor);
};

UENUM(BlueprintType)
enum class EArcVehicleSeatChangeType : uint8
{
	Invalid,
	EnterVehicle,
	ExitVehicle,
	SwitchSeats
};

struct ARCVEHICLES_API FArcVehicleScopedRelativeTransformRestoration
{
	FArcVehicleScopedRelativeTransformRestoration();
	FArcVehicleScopedRelativeTransformRestoration(AActor* InActor);
	~FArcVehicleScopedRelativeTransformRestoration();

	void Restore();

	TWeakObjectPtr<class AActor> OwnerActor;
	TMap<TWeakObjectPtr<class USceneComponent>, FTransform> ComponentTransformMap;
};