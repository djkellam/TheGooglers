// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FPS_Multiplayer.h"
#include "FPS_MultiplayerGameMode.h"
#include "FPS_MultiplayerHUD.h"
#include "FPS_MultiplayerCharacter.h"

AFPS_MultiplayerGameMode::AFPS_MultiplayerGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_MultiplayerHUD::StaticClass();
}

void AFPS_MultiplayerGameMode::Tick(float DeltaSeconds)
{
    
}

