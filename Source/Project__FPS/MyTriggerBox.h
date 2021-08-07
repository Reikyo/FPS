// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MyTriggerBox.generated.h"

UCLASS()
class PROJECT__FPS_API AMyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnTriggerEnter(AActor* actorThis, AActor* actorOther);
	UFUNCTION()
	void OnTriggerExit(AActor* actorThis, AActor* actorOther);
};
