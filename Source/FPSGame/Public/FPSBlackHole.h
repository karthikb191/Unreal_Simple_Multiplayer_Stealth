// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


#include "FPSBlackHole.generated.h"

UCLASS()
class FPSGAME_API AFPSBlackHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBlackHole();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USphereComponent *sphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent *meshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float radiusOfInfluence = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
