// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
//This must be included last
#include "FPSObjectiveActor.generated.h"

//class USphereComponent;	//Forward Declaration

UCLASS()
class FPSGAME_API AFPSObjectiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSObjectiveActor();

	//Provides a visual cue denoting the goal
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* meshComponent;

	//This is going to hold the collision information
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent *collisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Fx")
	UParticleSystem *proximityFx;

protected:
	UParticleSystemComponent *proximityFxComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlayEffects();

	UFUNCTION()
	void ClearObject();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Overriding the implementation that's available in the actor
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

};
