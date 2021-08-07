// Copyright 2021 Darren Temple

#include "MyActorHit.h"

// ------------------------------------------------------------------------------------------------

AMyActorHit::AMyActorHit()
{
	stmeshSelf = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("stmeshSelf"));
	stmeshSelf->SetSimulatePhysics(true);
	// stmeshSelf->OnComponentHit.AddDynamic(this, &AMyActorHit::OnHit);
}

// ------------------------------------------------------------------------------------------------

// // Good:
// void AMyActorHit::OnHit(
// 	UPrimitiveComponent* primSelf,
// 	AActor* actorOther,
// 	UPrimitiveComponent* primOther,
// 	FVector v3ForceImpulse,
// 	const FHitResult& hit
// )
// // Bad:
// // void AMyActorHit::OnHit(
// // 	UPrimitiveComponent* primSelf,
// // 	AActor* actorOther,
// // 	UPrimitiveComponent* primOther,
// // 	bool bSelfMoved,
// // 	FVector v3PosHit,
// // 	FVector v3NormalHit,
// // 	FVector v3ForceImpulse,
// // 	const FHitResult& hit
// // )
// {
// 	UE_LOG(LogTemp, Warning, TEXT("____OnHit: %s %s"), *this->GetName(), *actorOther->GetName());
// }

// ------------------------------------------------------------------------------------------------

// https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/NotifyHit/
//   "When receiving a hit from another object's movement (bSelfMoved is false), the directions of
//    'Hit.Normal' and 'Hit.ImpactNormal' will be adjusted to indicate force from the other object
//    against this object."

// Bad:
// void AMyActorHit::NotifyHit(
// 	UPrimitiveComponent* primSelf,
// 	AActor* actorOther,
// 	UPrimitiveComponent* primOther,
// 	FVector v3ForceImpulse,
// 	const FHitResult& hit
// )
// Good:
void AMyActorHit::NotifyHit(
	UPrimitiveComponent* primSelf,
	AActor* actorOther,
	UPrimitiveComponent* primOther,
	bool bSelfMoved,
	FVector v3PosHit,
	FVector v3NormalHit,
	FVector v3ForceImpulse,
	const FHitResult& hit
)
{
	UE_LOG(LogTemp, Warning, TEXT("NotifyHit: %s   %s   %s   %s   %s"),
		*this->GetName(),
		*actorOther->GetName(),
		*v3NormalHit.ToString(),
		*hit.Normal.ToString(),
		*hit.ImpactNormal.ToString()
	);
}

// ------------------------------------------------------------------------------------------------
