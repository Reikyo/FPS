// Copyright 2021 Darren Temple

#include "MyActor.h"

// ------------------------------------------------------------------------------------------------

AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// ------------------------------------------------------------------------------------------------

void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("MyActor ready to roll"));
}

// ------------------------------------------------------------------------------------------------

void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ------------------------------------------------------------------------------------------------
