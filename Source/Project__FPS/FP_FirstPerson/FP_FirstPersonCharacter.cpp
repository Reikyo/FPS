// Copyright Epic Games, Inc. All Rights Reserved.

#include "FP_FirstPersonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
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
	skmeshPlayerWeapon->bCastDynamicShadow = false;
	skmeshPlayerWeapon->CastShadow = false;
	skmeshPlayerWeapon->SetupAttachment(skmeshPlayerArms, TEXT("GripPoint"));

	fDegPerSecMaxLookHorizontal = 45.f;
	fDegPerSecMaxLookVertical = 45.f;

	v3OffsetWeapon = FVector(100.f, 30.f, 10.f);
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
	FVector v3RayStart = FVector::ZeroVector;

	if (playerController)
	{
		FRotator rotCamera;
		playerController->GetPlayerViewPoint(v3RayStart, rotCamera);
		v3DirWeapon = rotCamera.Vector();
		v3RayStart = v3RayStart + v3DirWeapon * ((GetActorLocation() - v3RayStart) | v3DirWeapon);
	}

	const FVector v3RayEnd = v3RayStart + (fRangeWeapon * v3DirWeapon);

	const FHitResult hit = RaycastWeapon(v3RayStart, v3RayEnd);

	AActor* actorHit = hit.GetActor();
	UPrimitiveComponent* primitiveHit = hit.GetComponent();

	if (	(actorHit != nullptr)
		&&	(actorHit != this)
		&&	(primitiveHit != nullptr)
		&&	(primitiveHit->IsSimulatingPhysics()) )
	{
		primitiveHit->AddImpulseAtLocation(fDamageWeapon * v3DirWeapon, hit.Location);
	}
}

// ------------------------------------------------------------------------------------------------

FHitResult AFP_FirstPersonCharacter::RaycastWeapon(const FVector& v3RayStart, const FVector& v3RayEnd) const
{
	FHitResult hit(ForceInit);
	FCollisionQueryParams paramsRay(SCENE_QUERY_STAT(RaycastWeapon), true, GetInstigator());
	paramsRay.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		hit,
		v3RayStart,
		v3RayEnd,
		COLLISION_WEAPON,
		paramsRay
	);

	return hit;
}

// ------------------------------------------------------------------------------------------------
