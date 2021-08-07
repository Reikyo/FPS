// Copyright 2021 Darren Temple

#include "MyTriggerBox.h"

// ------------------------------------------------------------------------------------------------

void AMyTriggerBox::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnTriggerEnter);
    OnActorEndOverlap.AddDynamic(this, &AMyTriggerBox::OnTriggerExit);
}

// ------------------------------------------------------------------------------------------------

void AMyTriggerBox::OnTriggerEnter(AActor* actorThis, AActor* actorOther)
{
    UE_LOG(LogTemp, Warning, TEXT("OnTriggerEnter: %s %s"), *actorThis->GetName(), *actorOther->GetName());
}

// ------------------------------------------------------------------------------------------------

void AMyTriggerBox::OnTriggerExit(AActor* actorThis, AActor* actorOther)
{
    UE_LOG(LogTemp, Warning, TEXT("OnTriggerExit: %s %s"), *actorThis->GetName(), *actorOther->GetName());
}

// ------------------------------------------------------------------------------------------------
