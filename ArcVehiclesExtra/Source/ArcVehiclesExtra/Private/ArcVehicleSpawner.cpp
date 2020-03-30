// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSpawner.h"
#include "Engine.h"
#include "ArcBaseVehicle.h"

// Sets default values
AArcVehicleSpawner::AArcVehicleSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

#if WITH_EDITOR
	EditorVehicleMesh = CreateEditorOnlyDefaultSubobject<USkeletalMeshComponent>("Editor Mesh");
	if (IsValid(EditorVehicleMesh))
	{
		EditorVehicleMesh->SetupAttachment(RootComponent);
		EditorVehicleMesh->bIsEditorOnly = true;
		EditorVehicleMesh->bHiddenInGame = true;
		EditorVehicleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
#endif
}

// Called when the game starts or when spawned
void AArcVehicleSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (bSpawnImmediately)
	{
		SpawnVehicle();
	}
	else
	{
		StartVehicleSpawnTimer();
	}

}

// Called every frame
void AArcVehicleSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArcVehicleSpawner::StartVehicleSpawnTimer()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(RespawnTimer))
	{
		
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AArcVehicleSpawner::SpawnVehicleTimerEnd, RespawnDelay);
	}
}

void AArcVehicleSpawner::SpawnVehicleTimerEnd()
{
	//TODO: Check if the space is clear.
	SpawnVehicle();
}

void AArcVehicleSpawner::SpawnVehicle()
{

	AArcBaseVehicle* Vehicle = GetWorld()->SpawnActorDeferred<AArcBaseVehicle>(VehicleClass, GetActorTransform());
	if (IsValid(Vehicle))
	{
		

		UGameplayStatics::FinishSpawningActor(Vehicle, GetActorTransform());

		Vehicle->OnDestroyed.AddDynamic(this, &AArcVehicleSpawner::OnVehicleDestroyed);
		TrackedVehicles.Add(Vehicle);
	}
}

void AArcVehicleSpawner::OnVehicleDestroyed(AActor* DestroyedActor)
{
	if (AArcBaseVehicle* Vehicle = Cast<AArcBaseVehicle>(DestroyedActor))
	{
		if (TrackedVehicles.Contains(Vehicle))
		{
			TrackedVehicles.Remove(Vehicle);

			if (TrackedVehicles.Num() < MaxVehiclesAlive)
			{
				StartVehicleSpawnTimer();
			}
		}
	}
}

#if WITH_EDITOR
void AArcVehicleSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AArcVehicleSpawner, VehicleClass) && IsValid(VehicleClass))
	{
		USkeletalMeshComponent* ASkeletalMeshComp = nullptr;

		UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(VehicleClass.Get());
		const TArray<USCS_Node*>& Nodes = BPClass->SimpleConstructionScript->GetAllNodes();
		for (auto Node : Nodes)
		{
			if (USkeletalMeshComponent* SKMesh = Cast<USkeletalMeshComponent>(Node->ComponentTemplate))
			{				
				ASkeletalMeshComp = SKMesh;		
				break;
			}
		}
		
		if (IsValid(ASkeletalMeshComp) && IsValid(EditorVehicleMesh))
		{
			EditorVehicleMesh->SetSkeletalMesh(ASkeletalMeshComp->SkeletalMesh);
		}
	}
}
#endif

