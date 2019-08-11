// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

#include "FPSAI.generated.h"

class UPawnSensingComponent;

//In enum classes, enumerator names are local to the enum and their values do not implicitly convert to other types
UENUM(BlueprintType)
enum class EAIState : uint8{
	Idle,
	Suspicious,
	Alerted
};



UCLASS()
class FPSGAME_API AFPSAI : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent *pawnSensingComponent;
	
	FRotator originalRotation;

	FTimerHandle timerHandle;

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState guardState;
	UFUNCTION()
	void OnRep_GuardState();

public:
	// Sets default values for this character's properties
	AFPSAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayerSeen(APawn* actorSeen);
	UFUNCTION()
	void NoiseHeard(APawn* actorSeen, const FVector& Location, float Volume);
	UFUNCTION()
	void ResetRotation();

	void SetGuardState(EAIState newState);
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState newState);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
