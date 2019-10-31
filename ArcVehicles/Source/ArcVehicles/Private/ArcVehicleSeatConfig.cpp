// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcVehicleSeatConfig.h"

#if WITH_EDITOR
#include "Kismet2/ComponentEditorUtils.h"
#endif

UArcVehicleSeatConfig::UArcVehicleSeatConfig()
{
	
}

void UArcVehicleSeatConfig::PostInitProperties()
{
	Super::PostInitProperties();
	
	//For now, restrict the component search to the owner actor.
	//We'll need do some fanagling to get it working with seat pawns

#if WITH_EDITOR
	if (AActor* OuterActor = Cast<AActor>(GetOuter()))
	{
		AttachComponent = FComponentEditorUtils::MakeComponentReference(OuterActor, OuterActor->GetRootComponent());
	}
#endif
	
}
