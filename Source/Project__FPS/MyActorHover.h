// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MyActorHover.generated.h"

UCLASS()
class PROJECT__FPS_API AMyActorHover : public AActor
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

private:
	FVector v3PosStart;
	FVector v3PosTarget;
	FTimeline timeline;
	UFUNCTION()
	void HandleProgress(float fValue);

// ------------------------------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;

// ------------------------------------------------------------------------------------------------

public:
	AMyActorHover();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* stMesh;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* curveFloat;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	float fPosZDeltaHover;

// ------------------------------------------------------------------------------------------------

};
