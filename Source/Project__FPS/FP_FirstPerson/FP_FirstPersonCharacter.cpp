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
	inputPlayer->BindAction("TestRaycastShape", IE_Pressed, this, &AFP_FirstPersonCharacter::TestRaycastShape);
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
	FVector v3PosStartRay = FVector::ZeroVector;

	if (playerController)
	{
		FRotator rotCamera;
		playerController->GetPlayerViewPoint(v3PosStartRay, rotCamera);
		v3DirWeapon = rotCamera.Vector();
		v3PosStartRay = v3PosStartRay + v3DirWeapon * ((GetActorLocation() - v3PosStartRay) | v3DirWeapon);
	}

	const FVector v3PosEndRay = v3PosStartRay + (fRangeWeapon * v3DirWeapon);

	const FHitResult hit = GetHitRay(v3PosStartRay, v3PosEndRay);
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

FHitResult AFP_FirstPersonCharacter::GetHitRay(const FVector& v3PosStartRay, const FVector& v3PosEndRay) const
{
	FHitResult hit(ForceInit);
	FCollisionQueryParams params(SCENE_QUERY_STAT(GetHitRay), true, GetInstigator());
	params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		hit,
		v3PosStartRay,
		v3PosEndRay,
		COLLISION_WEAPON,
		params
	);

	return hit;
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::TestRaycast()
{
	// https://www.orfeasel.com/single-line-raycasting
	// Single line raycasting
	// Orfeas Eleftheriou
	// 2015/12/23

	FHitResult hit;
	FCollisionQueryParams params(SCENE_QUERY_STAT(TestRaycast), true, GetInstigator()); // This version was taken from the FPS template, and discludes the player in the hit possibilities
	// FCollisionQueryParams params; // This version was from the original blog author, which includes the player in the hit possibilities

	float fLengthRay = 500.f;
    FVector v3PosStartRay = camFirstPerson->GetComponentLocation();
    FVector v3PosEndRay = v3PosStartRay + (fLengthRay * camFirstPerson->GetForwardVector());

	// This version was from the original blog author:
    // ActorLineTraceSingle(
	// 	hit,
	// 	v3PosStartRay,
	// 	v3PosEndRay,
	// 	ECollisionChannel::ECC_WorldDynamic,
	// 	params
	// );

	// This version was from a comment in the blog, as the author's version has issues with self hits,
	// even with the adjusted FCollisionQueryParams. This version is also more similar to the Unreal
	// FPS demo code:
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		hit,
		v3PosStartRay,
		v3PosEndRay,
		// ECollisionChannel::ECC_WorldStatic,
		ECollisionChannel::ECC_WorldDynamic,
		params
	);

	UE_LOG(LogTemp, Warning, TEXT("----- TestRaycast --------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.ToString());
	if (hit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.GetActor()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hit.GetComponent()->GetName());
	}

    DrawDebugLine(
		GetWorld(),
		v3PosStartRay,
		v3PosEndRay,
		FColor::Green,
		true, // Line persistence
		-1,
		0,
		1.f // Line width
	);
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::TestRaycastShape()
{
	// https://www.orfeasel.com/tracing-multiple-objects
	// Tracing multiple objects
	// Orfeas Eleftheriou
	// 2015/12/24

    TArray<FHitResult> hitArr;
	FCollisionQueryParams params(SCENE_QUERY_STAT(TestRaycastShape), true, GetInstigator());

	float fPosDeltaMagSphere = 500.f;
	float fRadiusSphere = 100.f;
	int iNumSegmentSphere = 32;
    // FVector v3PosStartSphere = GetActorLocation();
	FVector v3PosStartSphere = camFirstPerson->GetComponentLocation();
    FVector v3PosEndSphere = v3PosStartSphere + (fPosDeltaMagSphere * camFirstPerson->GetForwardVector());
	FQuat quatSphere = FQuat();

    FCollisionShape collisionShape;
    collisionShape.ShapeType = ECollisionShape::Sphere;
    collisionShape.SetSphere(fRadiusSphere);
	// UE_LOG(LogTemp, Warning, TEXT("%f"), collisionShape.GetSphereRadius());

	// This returns all initial objects that overlap with the shape in its starting position, plus the
	// first object that interacts with the shape as it moves to its end position is the final entry in
	// the array. Took me a while to get my head around what's going on, but hopefully this description
	// is relatively clear on re-reading.
    bool bHit = GetWorld()->SweepMultiByChannel(
		hitArr,
		v3PosStartSphere,
		v3PosEndSphere,
		quatSphere,
		// ECollisionChannel::ECC_WorldStatic,
		ECollisionChannel::ECC_WorldDynamic,
		collisionShape,
		params
	);

	UE_LOG(LogTemp, Warning, TEXT("----- TestRaycastShape ---------------------------"));
	if (!bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("No hit"));
	}
	else
	{
		for (auto hit = hitArr.CreateIterator(); hit; hit++)
	    {
	        // GLog->Log((*hit).Actor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("%s"), *hit->ToString());
			if (hit->IsValidBlockingHit())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *hit->GetActor()->GetName());
				UE_LOG(LogTemp, Warning, TEXT("%s"), *hit->GetComponent()->GetName());
			}
			UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"));
			// These three lines are all equivalent:
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *(*hit).Actor->GetName());
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *hit->Actor->GetName());
			// UE_LOG(LogTemp, Warning, TEXT("%s"), *hit->GetActor()->GetName());
	    }
	}

    DrawDebugSphere(
		GetWorld(),
		v3PosStartSphere,
		fRadiusSphere,
		iNumSegmentSphere,
		FColor::Green,
		true
	);

	DrawDebugSphere(
		GetWorld(),
		v3PosEndSphere,
		fRadiusSphere,
		iNumSegmentSphere,
		FColor::Blue,
		true
	);
}

// ------------------------------------------------------------------------------------------------

void AFP_FirstPersonCharacter::TestBlueprintCallable(int32 iMyInt)
{

}

// ------------------------------------------------------------------------------------------------
