// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MyAIController.generated.h"

UCLASS()
class PROJECT__FPS_API AMyAIController : public AAIController
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

public:
	AMyAIController();
	FORCEINLINE UBlackboardComponent* GetBlackboard() const { return blackboard; }
	FORCEINLINE TArray<AActor*> GetTargetPoints() { return myAITargetPointArr; }
	void SetSeenTarget(APawn* pawn);

// ------------------------------------------------------------------------------------------------

private:
	UBehaviorTreeComponent* behaviorTree;
	UBlackboardComponent* blackboard;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName keyPositionToGo;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName keyTarget;

	TArray<AActor*> myAITargetPointArr;

	virtual void OnPossess(APawn* pawn) override;

// ------------------------------------------------------------------------------------------------

};
