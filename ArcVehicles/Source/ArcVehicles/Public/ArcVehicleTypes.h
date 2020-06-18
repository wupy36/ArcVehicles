// Copyright 2017-2020 Puny Human, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArcVehicleTypes.generated.h"

class AArcBaseVehicle;
class UArcVehicleSeatConfig;

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

USTRUCT(BlueprintType)
struct ARCVEHICLES_API FArcVehicleSeatReference
{
	GENERATED_USTRUCT_BODY()
public:
	FArcVehicleSeatReference()
		: Vehicle(nullptr),
		SeatID(INDEX_NONE)
	{

	}

	FArcVehicleSeatReference(AArcBaseVehicle* InVehicle, int32 InSeatId)
		: Vehicle(InVehicle)
		, SeatID(InSeatId)
	{

	}

	FArcVehicleSeatReference(UArcVehicleSeatConfig* SeatConfig);

	friend class AArcBaseVehicle;

	UPROPERTY()
	AArcBaseVehicle* Vehicle;
	UPROPERTY()
	int32 SeatID;

	UArcVehicleSeatConfig* operator->();
	UArcVehicleSeatConfig* operator*();

	bool IsValid() const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	FString ToString() const;
};

template<>
struct TStructOpsTypeTraits<FArcVehicleSeatReference> : public TStructOpsTypeTraitsBase2<FArcVehicleSeatReference>
{
	enum
	{
		WithNetSerializer = true,
	};
};