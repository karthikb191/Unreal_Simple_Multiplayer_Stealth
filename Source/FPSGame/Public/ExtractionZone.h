// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExtractionZone.generated.h"

class UBoxComponent;
class UDecalComponent;
class UAudioComponent;
UCLASS()
class FPSGAME_API AExtractionZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExtractionZone();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* sound_ObjectiveNotPresent;

	UAudioComponent *audioComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UBoxComponent *boxComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
		UDecalComponent *decalComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void ActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void StopSound();
};
