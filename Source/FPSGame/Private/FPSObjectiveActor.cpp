// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSObjectiveActor.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacter.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent"));
	SetRootComponent(meshComponent);

	collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("collisionComponent"));
	collisionComponent->SetupAttachment(meshComponent);

	meshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Collision component ignores all the channels(Doesn't detect any queries or collision).
	collisionComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	//Tells the collision component to send the overlap queries
	collisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);



	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	//PlayEffects();

	//A delegate will be added so that an appropriate function is called upon object destruction
	FScriptDelegate f;
	f.BindUFunction(this, "ClearObject");
	OnDestroyed.AddUnique(f);
}

// Called every frame
void AFPSObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSObjectiveActor::PlayEffects() {
	
	if (!proximityFxComponent) {
		if (proximityFx) {
			proximityFxComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), proximityFx, GetActorLocation() + FVector(0, 0, 100), FRotator(), false);

			UE_LOG(LogTemp, Warning, TEXT("Component created"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("No Play Effect assigned to spawn"))
	}
	else
		proximityFxComponent->ActivateSystem();
}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);
	
	PlayEffects();
	
	//AFPSCharacter* c = Cast<AFPSCharacter>(OtherActor);
	//if (c) {
	//	
	//	UE_LOG(LogTemp, Warning, TEXT("Cast Successful.............."));
	//}
	//else {
	//	UE_LOG(LogTemp, Warning, TEXT("Cast Failed"));
	//}

}

void AFPSObjectiveActor::NotifyActorEndOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (proximityFxComponent) {
		proximityFxComponent->DeactivateSystem();
		UE_LOG(LogTemp, Warning, TEXT("Deactivated System"));
	}

	AFPSCharacter* c = Cast<AFPSCharacter>(OtherActor);
	if (c) {
		//removing the reference
	} 
	
	//OnDestroyed.__Internal_AddDynamic(this, &AFPSObjectiveActor::ClearObject, "ClearObject");
}

UFUNCTION()
void AFPSObjectiveActor::ClearObject() {
	if (proximityFxComponent) {
		proximityFxComponent->DeactivateSystem();
		//Particle system will also be destroyed along with the objective
		//proximityFxComponent->DestroyComponent();
		UE_LOG(LogTemp, Warning, TEXT("Deactivated System"));
	}
}