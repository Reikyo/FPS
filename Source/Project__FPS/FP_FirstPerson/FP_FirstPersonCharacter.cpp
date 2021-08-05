// Copyright Epic Games, Inc. All Rights Reserved.

#include "FP_FirstPersonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <stdlib.h>

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

// ------------------------------------------------------------------------------------------------

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	camFirstPerson = CreateDefaultSubobject<UCameraComponent>(TEXT("camFirstPerson"));
	camFirstPerson->SetupAttachment(GetCapsuleComponent());
	camFirstPerson->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	camFirstPerson->bUsePawnControlRotation = true;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for skmeshPlayerArms are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	skmeshPlayerArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skmeshPlayerArms"));
	skmeshPlayerArms->SetOnlyOwnerSee(true);
	skmeshPlayerArms->SetupAttachment(camFirstPerson);
	skmeshPlayerArms->bCastDynamicShadow = false;
	skmeshPlayerArms->CastShadow = false;

	skmeshPlayerWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skmeshPlayerWeapon"));
	skmeshPlayerWeapon->SetOnlyOwnerSee(true);
	skmeshPlayerWeapon->SetupAttachment(skmeshPlayerArms, TEXT("GripPoint"));
	skmeshPlayerWeapon->bCastDynamicShadow = false;
	skmeshPlayerWeapon->CastShadow = false;

	fDegPerSecMaxLookHorizontal = 45.f;
	fDegPerSecMaxLookVertical = 45.f;

	v3PosOffsetWeapon = FVector(100.f, 30.f, 10.f);
	fRangeWeapon = 5000.f;
	fDamageWeapon = 5000.f;
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* inputPlayer)
{
	check(inputPlayer);

	inputPlayer->BindAxis("MoveForwardBackward", this, &AFP_FirstPersonCharacter::MoveForwardBackward);
	inputPlayer->BindAxis("MoveRightLeft", this, &AFP_FirstPersonCharacter::MoveRightLeft);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently:
	//   Devices that we choose to treat as a rate of change, such as an analog joystick:
	inputPlayer->BindAxis("LookHorizontal1", this, &AFP_FirstPersonCharacter::LookHorizontal);
	inputPlayer->BindAxis("LookVertical1", this, &AFP_FirstPersonCharacter::LookVertical);
	//   Devices that provide an absolute delta, such as a mouse:
	inputPlayer->BindAxis("LookHorizontal2", this, &APawn::AddControllerYawInput);
	inputPlayer->BindAxis("LookVertical2", this, &APawn::AddControllerPitchInput);

	inputPlayer->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	inputPlayer->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	inputPlayer->BindAction("FireWeapon", IE_Pressed, this, &AFP_FirstPersonCharacter::FireWeapon);

	inputPlayer->BindAction("TestRaycast", IE_Pressed, this, &AFP_FirstPersonCharacter::TestRaycast);
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::MoveForwardBackward(float fMoveForwardBackward)
{
	if (abs(fMoveForwardBackward) > 0.f)
	{
		AddMovementInput(GetActorForwardVector(), fMoveForwardBackward);
	}
}

void AFP_FirstPersonCharacter::MoveRightLeft(float fMoveRightLeft)
{
	if (abs(fMoveRightLeft) > 0.f)
	{
		AddMovementInput(GetActorRightVector(), fMoveRightLeft);
	}
}

void AFP_FirstPersonCharacter::LookHorizontal(float fFractionDegPerSecMax)
{
	if (abs(fFractionDegPerSecMax) > 0.f)
	{
		AddControllerYawInput(fFractionDegPerSecMax * fDegPerSecMaxLookHorizontal * GetWorld()->GetDeltaSeconds());
	}
}

void AFP_FirstPersonCharacter::LookVertical(float fFractionDegPerSecMax)
{
	if (abs(fFractionDegPerSecMax) > 0.f)
	{
		AddControllerPitchInput(fFractionDegPerSecMax * fDegPerSecMaxLookVertical * GetWorld()->GetDeltaSeconds());
	}
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::FireWeapon()
{
	if (vfxclpFireWeapon != nullptr)
	{
		UAnimInstance* animInstance = skmeshPlayerArms->GetAnimInstance();
		if (animInstance != nullptr)
		{
			animInstance->Montage_Play(vfxclpFireWeapon, 1.f);
		}
	}

	if (sfxclpFireWeapon != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, sfxclpFireWeapon, GetActorLocation());
	}

	APlayerController* playerController = Cast<APlayerController>(GetController());
	FVector v3DirWeapon = FVector::ZeroVector;
	FVector v3PosRayStart = FVector::ZeroVector;

	if (playerController)
	{
		FRotator rotCamera;
		playerController->GetPlayerViewPoint(v3PosRayStart, rotCamera);
		v3DirWeapon = rotCamera.Vector();
		v3PosRayStart = v3PosRayStart + v3DirWeapon * ((GetActorLocation() - v3PosRayStart) | v3DirWeapon);
	}

	const FVector v3PosRayEnd = v3PosRayStart + (fRangeWeapon * v3DirWeapon);

	const FHitResult hit = GetHitRay(v3PosRayStart, v3PosRayEnd);
	AActor* actorHit = hit.GetActor();
	UPrimitiveComponent* componentHit = hit.GetComponent();

	if (	(actorHit != nullptr)
		&&	(actorHit != this)
		&&	(componentHit != nullptr)
		&&	(componentHit->IsSimulatingPhysics()) )
	{
		componentHit->AddImpulseAtLocation(fDamageWeapon * v3DirWeapon, hit.Location);
	}
}

// ------------------------------------------------------------------------------------------------

FHitResult AFP_FirstPersonCharacter::GetHitRay(const FVector& v3PosRayStart, const FVector& v3PosRayEnd) const
{
	FHitResult hit(ForceInit);
	FCollisionQueryParams paramsRay(SCENE_QUERY_STAT(GetHitRay), true, GetInstigator());
	paramsRay.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		hit,
		v3PosRayStart,
		v3PosRayEnd,
		COLLISION_WEAPON,
		paramsRay
	);

	return hit;
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::TestRaycast()
{
	// https://www.orfeasel.com/single-line-raycasting
	// Single Line Raycasting
	// Orfeas Eleftheriou
	// 2015/12/23

	float fLengthRay = 5000.f;
	FHitResult hit;
    FVector v3PosRayStart = camFirstPerson->GetComponentLocation();
    FVector v3PosRayEnd = v3PosRayStart + (fLengthRay * camFirstPerson->GetForwardVector());
	// FCollisionQueryParams paramsRay; // This version was from the original blog author, which includes the player in the hit possibilities
	FCollisionQueryParams paramsRay(SCENE_QUERY_STAT(TestRaycast), true, GetInstigator()); // This version was taken from the FPS template, and discludes the player in the hit possibilities

	// This version was from the original blog author:
    // ActorLineTraceSingle(
	// 	hit,
	// 	v3PosRayStart,
	// 	v3PosRayEnd,
	// 	ECollisionChannel::ECC_WorldDynamic,
	// 	paramsRay
	// );

	// This version was from a comment in the blog, as the author's version has issues with self hits,
	// even with the adjusted FCollisionQueryParams. This version is also more similar to the Unreal
	// FPS demo code:
	GetWorld()->LineTraceSingleByChannel(
		hit,
		v3PosRayStart,
		v3PosRayEnd,
		// ECollisionChannel::ECC_WorldStatic,
		ECollisionChannel::ECC_WorldDynamic,
		paramsRay
	);

    DrawDebugLine(
		GetWorld(),
		v3PosRayStart,
		v3PosRayEnd,
		FColor::Green,
		true, // Line persistence
		-1,
		0,
		1.f // Line width
	);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.ToString());
	if (hit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.GetComponent()->GetName());
	}
}

// ------------------------------------------------------------------------------------------------
