// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorToSpawn.h"
#include "MyActorSpawner.generated.h"

UCLASS()
class PROJECT__FPS_API AMyActorSpawner : public AActor
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;

// ------------------------------------------------------------------------------------------------

public:
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AMyActorToSpawn> BP_MyActorToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float fTimeWaitToSpawn = 5.f;

	UFUNCTION()
	void SpawnMyActorToSpawn();

// ------------------------------------------------------------------------------------------------

};
