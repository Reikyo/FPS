// Copyright 2021 Darren Temple

#include "MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyAICharacter.h"
#include "MyAITargetPoint.h"

// ------------------------------------------------------------------------------------------------

AMyAIController::AMyAIController()
{
    behaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("behaviorTree"));
    blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("blackboard"));
    keyPositionToGo = "PositionToGo";
}

// ------------------------------------------------------------------------------------------------

// OnPossess is executed when the character we want to control is spawned. Inside this function, we
// initialise the blackboard and start the behaviour tree.
void AMyAIController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    AMyAICharacter* myAICharacter = Cast<AMyAICharacter>(pawn);

    if (myAICharacter)
    {
        if (myAICharacter->behaviorTree->BlackboardAsset)
        {
            blackboard->InitializeBlackboard(*(myAICharacter->behaviorTree->BlackboardAsset));
        }
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyAITargetPoint::StaticClass(), myAITargetPointArr);
        behaviorTree->StartTree(*myAICharacter->behaviorTree);
    }
}

// ------------------------------------------------------------------------------------------------
