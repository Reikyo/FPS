// Copyright 2021 Darren Temple

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

UCLASS()
class PROJECT__FPS_API AMyAICharacter : public ACharacter
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

public:
	AMyAICharacter();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="AI")
	class UBehaviorTree* behaviorTree;

	UPROPERTY(VisibleAnywhere, Category="AI")
	class UPawnSensingComponent* pawnSensingComp;

private:
	UFUNCTION()
	void OnSeePlayer(APawn* pawn);

// ------------------------------------------------------------------------------------------------

// protected:
// 	virtual void BeginPlay() override;

// ------------------------------------------------------------------------------------------------

// public:
// 	virtual void Tick(float DeltaTime) override;
// 	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// ------------------------------------------------------------------------------------------------

};
