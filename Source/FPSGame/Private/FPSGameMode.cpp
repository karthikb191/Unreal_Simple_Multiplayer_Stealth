// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	//Define a game state class
	GameStateClass = AFPSGameState::StaticClass();
}


void AFPSGameMode::CompleteLevel(APawn* player, bool bMissionSuccess) {

	if (player) {
		//Player doesn't have to move once he reaches the extraction zone with target...This doesnt wrork for a multiplayer game
		//player->DisableInput(nullptr);


		TArray<AActor*> spectators;
		UGameplayStatics::GetAllActorsOfClass(this, spectatingViewPointClass, spectators);

		AActor *viewPawn = nullptr;
		if (spectators.Num()> 0)
			viewPawn = spectators[0];
		//Camera Navigation
		APlayerController *pc = Cast<APlayerController>(player->GetController());
		if (pc) {
			pc->SetViewTargetWithBlend(viewPawn, 1.0f, EViewTargetBlendFunction::VTBlend_Cubic);
		}

	}

	AFPSGameState *gs = GetGameState<AFPSGameState>();
	if (gs)
		gs->MultiCastCompleteLevel(player, bMissionSuccess);

	//OnMissionCompleted(player, bMissionSuccess);
	UE_LOG(LogTemp, Warning, TEXT("Mission Complete!"));
}
