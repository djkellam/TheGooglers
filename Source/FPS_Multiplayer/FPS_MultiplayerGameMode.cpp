// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FPS_Multiplayer.h"
#include "FPS_MultiplayerGameMode.h"
#include "FPS_MultiplayerHUD.h"
#include "FPS_MultiplayerCharacter.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"


AFPS_MultiplayerGameMode::AFPS_MultiplayerGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_MultiplayerHUD::StaticClass();
    LastManStanding = false;
    
    bReplicates = true;
}

void AFPS_MultiplayerGameMode::Tick(float DeltaSeconds)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Number of players: %i"), PlayerList.Num()));
    int count = 0;
    for (auto player : PlayerList)
    {
        if(player->dead)
        {
            count++;
        }
    }
    if(count == PlayerList.Num() - 1 && PlayerList.Num() > 1)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameOver"));
        LastManStanding = true;
    }
}

void AFPS_MultiplayerGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AFPS_MultiplayerGameMode, LastManStanding);
}

