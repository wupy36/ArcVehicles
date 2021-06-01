// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcVehicleSpawner.generated.h"

class AArcBaseVehicle;

UCLASS()
class ARCVEHICLESEXTRA_API AArcVehicleSpawner : public AActor
{
	GENERATED_BODY()

		UPROPERTY()
	UChildActorComponent* EditorVehicleMesh;

public:	
	// Sets default values for this actor's properties
	AArcVehicleSpawner();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Respawn")
	TSubclassOf<AArcBaseVehicle> VehicleClass;

	//If true, spawn a vehicle right at map start
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Respawn")
	bool bSpawnImmediately;

	//Time, in seconds, before respawning a vehicle
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Respawn")
	float RespawnDelay;

	//Max number of vehicles alive at any one time
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Respawn")
	int32 MaxVehiclesAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartVehicleSpawnTimer();
	UFUNCTION()
	virtual void SpawnVehicleTimerEnd();
	virtual void SpawnVehicle();

	UFUNCTION()
	virtual void OnVehicleDestroyed(AActor* DestroyedActor);

protected:
	TArray<AArcBaseVehicle*> TrackedVehicles;

	FTimerHandle RespawnTimer;

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
};
