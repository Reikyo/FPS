// Copyright 2021 Darren Temple

#include "MyActorHover.h"
#include "Components/TimelineComponent.h"

// ------------------------------------------------------------------------------------------------

AMyActorHover::AMyActorHover()
{
	PrimaryActorTick.bCanEverTick = true;
	stMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("stMesh"));
}

// ------------------------------------------------------------------------------------------------

void AMyActorHover::BeginPlay()
{
	Super::BeginPlay();

	if (curveFloat)
	{
		v3PosStart = v3PosTarget = GetActorLocation();
		v3PosTarget.Z += fPosZDeltaHover;
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleProgress"));
		timeline.AddInterpFloat(curveFloat, ProgressFunction);
		timeline.SetLooping(true);
		timeline.PlayFromStart();
	}
}

// ------------------------------------------------------------------------------------------------

void AMyActorHover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeline.TickTimeline(DeltaTime);
}

// ------------------------------------------------------------------------------------------------

void AMyActorHover::HandleProgress(float fValue)
{
	FVector v3PosNew = FMath::Lerp(v3PosStart, v3PosTarget, fValue);
	SetActorLocation(v3PosNew);
}

// ------------------------------------------------------------------------------------------------
