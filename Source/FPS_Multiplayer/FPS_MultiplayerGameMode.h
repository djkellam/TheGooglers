// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "FPS_MultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class AFPS_MultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AFPS_MultiplayerGameMode();
    
    virtual void Tick(float DeltaSeconds) override;
};



