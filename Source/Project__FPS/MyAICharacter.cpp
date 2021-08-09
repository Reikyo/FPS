// Copyright 2021 Darren Temple

#include "MyAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MyAIController.h"
#include "Perception/PawnSensingComponent.h"

// ------------------------------------------------------------------------------------------------

AMyAICharacter::AMyAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	pawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("pawnSensingComp"));
	pawnSensingComp->SetPeripheralVisionAngle(90.f);
}

// ------------------------------------------------------------------------------------------------

void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (pawnSensingComp)
	{
		pawnSensingComp->OnSeePawn.AddDynamic(this, &AMyAICharacter::OnSeePlayer);
	}
}

// ------------------------------------------------------------------------------------------------

// void AMyAICharacter::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// 	if (pawnSensingComp)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("%i"), pawnSensingComp->HasLineOfSightTo(GetWorld()->GetFirstPlayerController()->GetPawn()));
// 	}
// }

// ------------------------------------------------------------------------------------------------

// void AMyAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
// }

// ------------------------------------------------------------------------------------------------

void AMyAICharacter::OnSeePlayer(APawn* pawn)
{
	AMyAIController* myAIController = Cast<AMyAIController>(GetController());
	if (myAIController)
	{
		myAIController->SetSeenTarget(pawn);
	}
}

// ------------------------------------------------------------------------------------------------
