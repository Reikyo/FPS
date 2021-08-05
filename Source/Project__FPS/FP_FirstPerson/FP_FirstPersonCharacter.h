// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_FirstPersonCharacter.generated.h"

class UAnimMontage;
class UCameraComponent;
class UInputComponent;
class USkeletalMeshComponent;
class USoundBase;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

// ------------------------------------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* camFirstPerson;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* skmeshPlayerArms;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* skmeshPlayerWeapon;

// ------------------------------------------------------------------------------------------------

public:

	AFP_FirstPersonCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float fDegPerSecMaxLookHorizontal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float fDegPerSecMaxLookVertical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	FVector v3PosOffsetWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float fRangeWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float fDamageWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	UAnimMontage* vfxclpFireWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	USoundBase* sfxclpFireWeapon;

// ------------------------------------------------------------------------------------------------

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* inputPlayer) override;

	void MoveForwardBackward(float fMoveForwardBackward);
	void MoveRightLeft(float fMoveRightLeft);
	void LookHorizontal(float fFractionDegPerSecMax);
	void LookVertical(float fFractionDegPerSecMax);

	void FireWeapon();
	FHitResult GetHitRay(const FVector& v3PosRayStart, const FVector& v3PosRayEnd) const;

	void TestRaycast();

// ------------------------------------------------------------------------------------------------

public:

	FORCEINLINE class UCameraComponent* Get_camFirstPerson() const { return camFirstPerson; }
	FORCEINLINE class USkeletalMeshComponent* Get_skmeshPlayerArms() const { return skmeshPlayerArms; }

// ------------------------------------------------------------------------------------------------

};
