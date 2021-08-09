// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_SelectMyAITargetPoint.generated.h"

UCLASS()
class PROJECT__FPS_API UBT_SelectMyAITargetPoint : public UBTTaskNode
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner, uint8* nodeMemory) override;

// ------------------------------------------------------------------------------------------------

};
