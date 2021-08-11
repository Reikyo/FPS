// Copyright 2021 Darren Temple

#include "MyActorSpawner.h"

// ------------------------------------------------------------------------------------------------

void AMyActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		timerHandle,
		this,
		&AMyActorSpawner::SpawnMyActorToSpawn,
		fTimeWaitToSpawn
	);
}

// ------------------------------------------------------------------------------------------------

void AMyActorSpawner::SpawnMyActorToSpawn()
{
	if (BP_MyActorToSpawn)
	{
		// We can use the spawn parameters for a number of things e.g. disabling collision or adjusting the
		// spawn position if a collision is happening at the spawn point etc.
		FActorSpawnParameters actorSpawnParams;
		AMyActorToSpawn* myActorToSpawn = GetWorld()->SpawnActor<AMyActorToSpawn>(
			BP_MyActorToSpawn,
			GetTransform(),
			actorSpawnParams
		);
	}
}

// ------------------------------------------------------------------------------------------------
