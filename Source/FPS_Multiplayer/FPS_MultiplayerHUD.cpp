// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FPS_Multiplayer.h"
#include "FPS_MultiplayerHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "FPS_MultiplayerGameMode.h"
#include "Engine.h"

AFPS_MultiplayerHUD::AFPS_MultiplayerHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshiarTexObj.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = HUDFontOb.Object;
    
    bReplicates = true;
}


void AFPS_MultiplayerHUD::DrawHUD()
{
	Super::DrawHUD();
    
    // Draw very simple crosshair
    
    // find center of the Canvas
    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
    
    // offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
    const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5) + 10),
                                          (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f) + 15) );
    
    // draw the crosshair
    FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem( TileItem );
    
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Test 1"));
    AFPS_MultiplayerGameMode* GameMode = Cast<AFPS_MultiplayerGameMode>(UGameplayStatics::GetGameMode(this));
    if(GameMode)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Test 2"));
        if(GameMode->LastManStanding == true)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Test 3"));

            FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);
            FVector2D GameOverSize;
            GetTextSize(TEXT("GAME OVER"), GameOverSize.X, GameOverSize.Y, HUDFont);
            DrawText(TEXT("GAME OVER"), FColor::White, (ScreenDimensions.X - GameOverSize.X) / 2.0f, (ScreenDimensions.Y - GameOverSize.Y) / 2.0f, HUDFont);
        }
        
    }
    //DrawHUDServer();

}



/*void AFPS_MultiplayerHUD::DrawHUDServer_Implementation()
{
    //if(Role == ROLE_Authority)
    //{
    

    //}
}

bool AFPS_MultiplayerHUD::DrawHUDServer_Validate()
{
    return true;
}*/

