// Copyright 2021 Darren Temple

#include "BT_SelectMyAITargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyAIController.h"
#include "MyAITargetPoint.h"

// ------------------------------------------------------------------------------------------------

EBTNodeResult::Type UBT_SelectMyAITargetPoint::ExecuteTask(UBehaviorTreeComponent& owner, uint8* nodeMemory)
{
    AMyAIController* myAIController = Cast<AMyAIController>(owner.GetAIOwner());

    if (myAIController)
    {
        UBlackboardComponent* blackboard = myAIController->GetBlackboard();
        TArray<AActor*> myAITargetPointArr = myAIController->GetTargetPoints();
        int32 iIdx_myAITargetPointArr;
        AMyAITargetPoint* myAITargetPointCurrent = Cast<AMyAITargetPoint>(blackboard->GetValueAsObject("PositionToGo"));
        AMyAITargetPoint* myAITargetPointNext = nullptr;

        do
        {
            iIdx_myAITargetPointArr = FMath::RandRange(0, myAITargetPointArr.Num()-1);
            myAITargetPointNext = Cast<AMyAITargetPoint>(myAITargetPointArr[iIdx_myAITargetPointArr]);
        } while (myAITargetPointNext == myAITargetPointCurrent);

        blackboard->SetValueAsObject("PositionToGo", myAITargetPointNext);

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

// ------------------------------------------------------------------------------------------------
