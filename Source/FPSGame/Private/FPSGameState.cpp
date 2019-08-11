// Fill out your copyright notice in the Description page of Project Settings.
#include "FPSGameState.h"

#include "FPSPlayerController.h"
#include "Engine/World.h"


void AFPSGameState::MultiCastCompleteLevel_Implementation(APawn* player, bool bMissionSuccess) {

	//Loop ovre all the pawns that exist in this level
	//for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) {
	//	APawn *pawn = It->Get();
	//
	//	//Input is disabled if the pawn is locally controlled
	//	if (pawn && pawn->IsLocallyControlled()) {
	//		pawn->DisableInput(nullptr);
	//	}
	//}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		AFPSPlayerController *pc = Cast<AFPSPlayerController>(It->Get());

		if (pc && pc->IsLocalController()) {
			UE_LOG(LogTemp, Warning, TEXT("Local COntroller"));
			//Disable input
			APawn* p = pc->GetPawn();
			if (p)
				p->DisableInput(pc);

			pc->OnCompleteLevel(player, bMissionSuccess);

		}
		

	}

}