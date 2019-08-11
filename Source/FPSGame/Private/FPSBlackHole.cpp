// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSBlackHole.h"

#include "Kismet/GameplayStatics.h"

void TestDestroy(AActor* parent, AActor *victim);
// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(meshComponent);
	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));

	sphereComponent->SetupAttachment(meshComponent);
	
	meshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	sphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	sphereComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	sphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	sphereComponent->SetSphereRadius(radiusOfInfluence);
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float radialForce = 3000;

	TSet<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors);

	AActor *temp = this->GetOwner();
	//loop through the overlapping actors and add force
	for (auto& hit : overlappingActors) {
		UStaticMeshComponent *mesh = Cast<UStaticMeshComponent>(hit->GetRootComponent());

		if (mesh) {
			mesh->AddRadialForce(GetActorLocation(), radiusOfInfluence, -radialForce, ERadialImpulseFalloff::RIF_Constant, true);
			
			//UE_LOG(LogTemp, Warning, TEXT("Owner: %d  "), (temp == nullptr));
			TestDestroy(this, mesh->GetOwner());
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No Mesh to add force to"));
		}

	}

}

void TestDestroy(AActor* parent, AActor *victim) {
	if (parent && victim) {
		float distance = FVector::Distance(parent->GetTargetLocation(), victim->GetTargetLocation());
		if (distance < 100)
			victim->Destroy();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Parent: %d  .... victim: %d"), (parent==nullptr), (victim == nullptr));
}

