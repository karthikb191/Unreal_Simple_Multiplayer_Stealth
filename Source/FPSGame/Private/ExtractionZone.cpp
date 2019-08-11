// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtractionZone.h"

#include "Public/TimerManager.h"
#include "FPSGameMode.h"
#include "FPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
AExtractionZone::AExtractionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	boxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	boxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	boxComponent->SetBoxExtent(FVector(100, 100, 100));

	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AExtractionZone::ActorOverlap);

	decalComponent = CreateDefaultSubobject<UDecalComponent>("DecalComp");
	decalComponent->SetupAttachment(boxComponent);
	decalComponent->DecalSize = FVector(100.0f);

}

// Called when the game starts or when spawned
void AExtractionZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExtractionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
}

FTimerHandle timerHandle;

//The overlap override function only runs on the servre side
void AExtractionZone::ActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult){
	UE_LOG(LogTemp, Warning, TEXT("Actor overlap successful"));

	//Check if the character is carrying the object
	AFPSCharacter *character = Cast<AFPSCharacter>(OtherActor);

	if (character == nullptr)
		return;

	UE_LOG(LogTemp, Error, TEXT("Carrying OBJ %s %d"), *character->GetName(), character->bIsCarryingObjective);

	if (character->bIsCarryingObjective) {
		//Signal end game
		AFPSGameMode *gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

		//If the game mode cast is successful, then signal the level end. This check will fail on any of the clients
		if (gm)
			gm->CompleteLevel(character, true);
		else
			UE_LOG(LogTemp, Error, TEXT("Game mode cast failed!"));
	}
	else {
		if (sound_ObjectiveNotPresent) {
			
			if (!audioComponent) {
				audioComponent = UGameplayStatics::SpawnSoundAtLocation(this, sound_ObjectiveNotPresent, this->GetActorLocation());
				GetWorldTimerManager().SetTimer(timerHandle, this, &AExtractionZone::StopSound, 5.0f);
			}
		}
		
	}
}

void AExtractionZone::StopSound() {
	if (audioComponent) {
		audioComponent->FadeOut(2.0f, 0);
		audioComponent = nullptr;
	}
}

