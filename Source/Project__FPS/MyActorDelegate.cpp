// Copyright 2021 Darren Temple

#include "MyActorDelegate.h"

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::BeginPlay()
{
	Super::BeginPlay();

	myDelegateNoParams.BindUFunction(this, FName("FunctionA"));
	if (myDelegateNoParams.IsBound())
	{
		myDelegateNoParams.Execute();
	}

	myDelegateOneParam1.BindUFunction(this, FName("FunctionB"));
	if (myDelegateOneParam1.IsBound())
	{
		myDelegateOneParam1.Execute(10);
	}

	// If we try to bind multiple functions to a delegate that only accepts one, then only the last
	// given function will actually be bound:
	myDelegateOneParam2.BindUFunction(this, FName("FunctionB"));
	myDelegateOneParam2.BindUFunction(this, FName("FunctionC"));
	if (myDelegateOneParam2.IsBound())
	{
		myDelegateOneParam2.Execute(20);
	}

	myMulticastDelegateOneParam.AddUFunction(this, FName("FunctionB"));
	myMulticastDelegateOneParam.AddUFunction(this, FName("FunctionC"));
	if (myMulticastDelegateOneParam.IsBound())
	{
		myMulticastDelegateOneParam.Broadcast(30);
	}

	myDelegateTwoParams.BindUFunction(this, FName("FunctionD"));
	if (myDelegateTwoParams.IsBound())
	{
		myDelegateTwoParams.Execute(40, 50.f);
	}

	myDelegateThreeParams.BindUFunction(this, FName("FunctionE"));
	if (myDelegateThreeParams.IsBound())
	{
		myDelegateThreeParams.Execute(60, 70.f, true);
	}
}

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::FunctionA()
{
	UE_LOG(LogTemp, Warning, TEXT("FunctionA"));
}

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::FunctionB(int32 x)
{
	UE_LOG(LogTemp, Warning, TEXT("FunctionB given value %i"), x);
}

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::FunctionC(int32 x)
{
	UE_LOG(LogTemp, Warning, TEXT("FunctionC given value %i"), x);
}

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::FunctionD(int32 x, float y)
{
	UE_LOG(LogTemp, Warning, TEXT("FunctionD given values %i %f"), x, y);
}

// ------------------------------------------------------------------------------------------------

void AMyActorDelegate::FunctionE(int32 x, float y, bool z)
{
	UE_LOG(LogTemp, Warning, TEXT("FunctionE given values %i %f %s"), x, y, ( z ? TEXT("true") : TEXT("false") ));
}

// ------------------------------------------------------------------------------------------------
