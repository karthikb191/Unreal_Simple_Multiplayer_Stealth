// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAI.h"
#include "Engine/World.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "UObject/WeakObjectPtr.h"
#include "TimerManager.h"
#include "FPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSAI::AFPSAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	guardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAI::BeginPlay()
{
	Super::BeginPlay();
	
	//These events are executed only on server by default.
	pawnSensingComponent->OnSeePawn.AddDynamic(this, &AFPSAI::PlayerSeen);
	pawnSensingComponent->OnHearNoise.AddDynamic(this, &AFPSAI::NoiseHeard);

	originalRotation = GetActorRotation();
}

// Called every frame
void AFPSAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSAI::PlayerSeen(APawn* actorSeen) {
	if (actorSeen == nullptr)
		return;

	DrawDebugSphere(GetWorld(), actorSeen->GetActorLocation(), 30, 12, FColor::Magenta, false, 10.0f);

	//IF the player is spotted, mission failed
	AFPSGameMode *gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (gm)
		gm->CompleteLevel(actorSeen, false);

	guardState = EAIState::Alerted;
	//Notify the blueprint that the state had changed
	OnStateChanged(guardState);
}

void AFPSAI::NoiseHeard(APawn* actorSeen, const FVector& Location, float Volume) {

	if (guardState == EAIState::Alerted)
		return;

	FVector direction = Location - GetActorLocation();
	FRotator newRotation = FRotationMatrix::MakeFromX(direction).Rotator();
	newRotation.Roll = 0;
	newRotation.Pitch = 0;
	//Actor must look at the sound.
	SetActorRotation(newRotation);

	GetWorldTimerManager().ClearTimer(timerHandle);
	GetWorldTimerManager().SetTimer(timerHandle, this, &AFPSAI::ResetRotation, 3.0f);

	DrawDebugSphere(GetWorld(), Location, 30, 12, FColor::Emerald, false, 10.0f);

	guardState = EAIState::Suspicious;
	//Notify the blueprint that the state had changed
	OnStateChanged(guardState);
}


void AFPSAI::ResetRotation() {
	if (guardState == EAIState::Alerted)
		return;

	SetActorRotation(originalRotation);
	SetGuardState(EAIState::Idle);
}

void AFPSAI::SetGuardState(EAIState newState)
{
	if (guardState == newState)
		return;

	guardState = newState;
	//Notify the blueprint that the state had changed
	OnStateChanged(guardState);

	OnRep_GuardState();
}

//This function only gets called on the client machines
void AFPSAI::OnRep_GuardState()
{
	OnStateChanged(guardState);
}
void AFPSAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAI, guardState);	//Default rule replicates the change to all the clients
}

