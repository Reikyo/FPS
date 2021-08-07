// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class PROJECT__FPS_API AMyActor : public AActor
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

public:
	AMyActor();

// ------------------------------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;

// ------------------------------------------------------------------------------------------------

public:
	virtual void Tick(float DeltaTime) override;

// ------------------------------------------------------------------------------------------------

};
