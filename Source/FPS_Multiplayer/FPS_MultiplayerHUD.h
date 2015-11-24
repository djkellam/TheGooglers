// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "FPS_MultiplayerHUD.generated.h"

UCLASS()
class AFPS_MultiplayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFPS_MultiplayerHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
    
    /*UFUNCTION(Reliable, Server, WithValidation)
    void DrawHUDServer();
    void DrawHUDServer_Implementation();
    bool DrawHUDServer_Validate();*/

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
    
    //Variable for storing the font
    UPROPERTY()
    UFont * HUDFont;

};

