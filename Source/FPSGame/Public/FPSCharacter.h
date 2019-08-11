// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSObjectiveActor.h"

#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class UPawnNoiseEmitterComponent;

UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMeshComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	//Object that the player is currently touching
	UPROPERTY(Replicated)
	AFPSObjectiveActor *targetInContact;

	//Emitter is needed for detector to detect the sound
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnNoiseEmitterComponent* noiseEmitterComponent;

public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	//Must be changed on the server. Change will also be reflected on the children because it's replicated
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
	bool bIsCarryingObjective = false;
	
	/*Get the UClass reference of the objective class*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<AFPSObjectiveActor> objUClass;

protected:
	
	
	/** Fires a projectile. */
	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

	
	void TogglePickUp();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTogglePickUp(bool spawn);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAssignPickUp(AFPSObjectiveActor* target);


	AFPSObjectiveActor* GetTargetInContact();

	virtual void Tick(float DeltaTime) override;
};

