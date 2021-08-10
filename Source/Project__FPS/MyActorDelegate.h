// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorDelegate.generated.h"

UCLASS()
class PROJECT__FPS_API AMyActorDelegate : public AActor
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

private:
	DECLARE_DELEGATE(MyDelegateNoParams)
	DECLARE_DELEGATE_OneParam(MyDelegateOneParam, int32)
	DECLARE_DELEGATE_TwoParams(MyDelegateTwoParams, int32, float)
	DECLARE_DELEGATE_ThreeParams(MyDelegateThreeParams, int32, float, bool)
	DECLARE_MULTICAST_DELEGATE_OneParam(MyMulticastDelegateOneParam, int32)
	MyDelegateNoParams myDelegateNoParams;
	MyDelegateOneParam myDelegateOneParam1;
	MyDelegateOneParam myDelegateOneParam2;
	MyDelegateTwoParams myDelegateTwoParams;
	MyDelegateThreeParams myDelegateThreeParams;
	MyMulticastDelegateOneParam myMulticastDelegateOneParam;

// ------------------------------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;

// ------------------------------------------------------------------------------------------------

public:
	UFUNCTION()
	void FunctionA();

	UFUNCTION()
	void FunctionB(int32 x);

	UFUNCTION()
	void FunctionC(int32 x);

	UFUNCTION()
	void FunctionD(int32 x, float y);

	UFUNCTION()
	void FunctionE(int32 x, float y, bool z);

// ------------------------------------------------------------------------------------------------

};
