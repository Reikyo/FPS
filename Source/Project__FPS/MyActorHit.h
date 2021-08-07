// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorHit.generated.h"

UCLASS()
class PROJECT__FPS_API AMyActorHit : public AActor
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

public:
	AMyActorHit();

// ------------------------------------------------------------------------------------------------

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* stmeshSelf;

	// Good:
	// UFUNCTION()
	// void OnHit(
	// 	UPrimitiveComponent* primSelf,
	// 	AActor* actorOther,
	// 	UPrimitiveComponent* primOther,
	// 	FVector v3ForceImpulse,
	// 	const FHitResult& hit
	// );

	// Bad:
	// UFUNCTION()
	// void OnHit(
	// 	UPrimitiveComponent* primSelf,
	// 	AActor* actorOther,
	// 	UPrimitiveComponent* primOther,
	// 	bool bSelfMoved,
	// 	FVector v3PosHit,
	// 	FVector v3NormalHit,
	// 	FVector v3ForceImpulse,
	// 	const FHitResult& hit
	// );

	// Bad:
	// void NotifyHit(
	// 	UPrimitiveComponent* primSelf,
	// 	AActor* actorOther,
	// 	UPrimitiveComponent* primOther,
	// 	FVector v3ForceImpulse,
	// 	const FHitResult& hit
	// );

	// Good:
	UFUNCTION()
	void NotifyHit(
		UPrimitiveComponent* primSelf,
		AActor* actorOther,
		UPrimitiveComponent* primOther,
		bool bSelfMoved,
		FVector v3PosHit,
		FVector v3NormalHit,
		FVector v3ForceImpulse,
		const FHitResult& hit
	);

// ------------------------------------------------------------------------------------------------

};
