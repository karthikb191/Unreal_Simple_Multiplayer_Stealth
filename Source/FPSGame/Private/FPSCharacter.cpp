// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"

#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSObjectiveActor.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->RelativeRotation = FRotator(2.0f, -15.0f, 5.0f);
	Mesh1PComponent->RelativeLocation = FVector(0, 0, -160.0f);

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");


	noiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	bReplicates = true;
}

//Setting the player's target in contact
void AFPSCharacter::NotifyActorBeginOverlap(AActor *other) {
	if (IsLocallyControlled()) {
		AFPSObjectiveActor* c = Cast<AFPSObjectiveActor>(other);
		if (c) {
			UE_LOG(LogTemp, Warning, TEXT("Overlapping with objective %s"), *this->GetName());
			//Setting the target character to this actor's address
			//targetInContact = c;
			ServerAssignPickUp(c);
		}
	}
}
void AFPSCharacter::NotifyActorEndOverlap(AActor *other) {
	if (IsLocallyControlled()) {
		AFPSObjectiveActor* c = Cast<AFPSObjectiveActor>(other);
	
		if (c != nullptr && c == targetInContact) {
			UE_LOG(LogTemp, Warning, TEXT("Overlap exit with objective"));
			//Setting the target character to this actor's address
			//targetInContact = NULL;
			ServerAssignPickUp(NULL);
		}
	}
}

//Server Implements this
void AFPSCharacter::ServerAssignPickUp_Implementation(AFPSObjectiveActor* target) {
	//This assignment is also reflected to the clients
	UE_LOG(LogTemp, Warning, TEXT("Target Set....................... %s"), *this->GetName());
	targetInContact = target;
}

bool AFPSCharacter::ServerAssignPickUp_Validate(AFPSObjectiveActor* target) {
	return true;
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PickUp", EInputEvent::IE_Pressed, this, &AFPSCharacter::TogglePickUp);
}


void AFPSCharacter::Fire()
{
	ServerFire();

	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}

void AFPSCharacter::ServerFire_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass)
	{
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = GunMeshComponent->GetSocketRotation("Muzzle");

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		//Everything that the bullet does is instigated by this actor
		ActorSpawnParams.Instigator = this;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);

	}
}
bool AFPSCharacter::ServerFire_Validate()
{
	return true;
}

void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	//We want to execute this only when the character is not locally controlled to replicate the looking up and down features
	if (!IsLocallyControlled()) {
		FRotator newRotation = CameraComponent->RelativeRotation;
		newRotation.Pitch = RemoteViewPitch * 360.f/ 255.f;
		CameraComponent->SetRelativeRotation(newRotation);
	}

}


void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::TogglePickUp() {
	
	ServerTogglePickUp(false);
}

//This is executed on the server. Destruction and creation of objects are reflected on the clients also
void AFPSCharacter::ServerTogglePickUp_Implementation(bool spawn) {
	if (!bIsCarryingObjective) {
		if (targetInContact) {
			UE_LOG(LogTemp, Warning, TEXT("Check...... %s"), *this->GetName());
			targetInContact->Destroy();
			bIsCarryingObjective = true;
			targetInContact = NULL;
			UE_LOG(LogTemp, Warning, TEXT("Destroyed One"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("No Target in contact"));
	}
	else {
		//DROP him
		//FVector location = (GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100);
		FVector location = GetActorLocation() + GetActorForwardVector() * 300;
		location.Z -= 100;
		UE_LOG(LogTemp, Warning, TEXT("Droppping at %s"), *location.ToString());
		FRotator rotation(0, 0, 0);
		FActorSpawnParameters spawnSparameters;
		spawnSparameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GetWorld()->SpawnActor<AFPSObjectiveActor>(objUClass, location, rotation, spawnSparameters);

		bIsCarryingObjective = false;
	}
}

bool AFPSCharacter::ServerTogglePickUp_Validate(bool spawn) {
	return true;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, targetInContact);
	//DOREPLIFETIME_CONDITION(AFPSCharacter, bIsCarryingObjective, COND_OwnerOnly);
	DOREPLIFETIME(AFPSCharacter, bIsCarryingObjective);
}


AFPSObjectiveActor* AFPSCharacter::GetTargetInContact(){
	return targetInContact;
}

