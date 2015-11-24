// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FPS_Multiplayer.h"
#include "FPS_MultiplayerCharacter.h"
#include "FPS_MultiplayerProjectile.h"
#include "Engine.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPS_MultiplayerCharacter

AFPS_MultiplayerCharacter::AFPS_MultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
    
    bReplicates = true;
    
    HP = 100;
    
    dead = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPS_MultiplayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_MultiplayerCharacter::TouchStarted);
	if( EnableTouchscreenMovement(InputComponent) == false )
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_MultiplayerCharacter::OnFireServer);
	}
	
	InputComponent->BindAxis("MoveForward", this, &AFPS_MultiplayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFPS_MultiplayerCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AFPS_MultiplayerCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AFPS_MultiplayerCharacter::LookUpAtRate);
}

void AFPS_MultiplayerCharacter::OnFire()
{
    // try and fire a projectile
    if (ProjectileClass != NULL)
    {
        const FRotator SpawnRotation = GetControlRotation();
        // MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
        const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

        UWorld* const World = GetWorld();
        if (World != NULL)
        {
            // spawn the projectile at the muzzle
            World->SpawnActor<AFPS_MultiplayerProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
        }
    }
    
}

void AFPS_MultiplayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPS_MultiplayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AFPS_MultiplayerCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AFPS_MultiplayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPS_MultiplayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPS_MultiplayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPS_MultiplayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFPS_MultiplayerCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_MultiplayerCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPS_MultiplayerCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPS_MultiplayerCharacter::TouchUpdate);
	}
	return bResult;
}


void AFPS_MultiplayerCharacter::OnFireServer_Implementation()
{
    // try and play the sound if specified
    if (FireSound != NULL)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }
    
    // try and play a firing animation if specified
    if(FireAnimation != NULL)
    {
        // Get the animation object for the arms mesh
        UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
        if(AnimInstance != NULL)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.f);
        }
    }

    if(Role == ROLE_Authority)
    {
        OnFire();
    }
}

bool AFPS_MultiplayerCharacter::OnFireServer_Validate()
{
    return true;
}

void AFPS_MultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AFPS_MultiplayerCharacter, ProjectileClass);
    DOREPLIFETIME(AFPS_MultiplayerCharacter, HP);
    DOREPLIFETIME(AFPS_MultiplayerCharacter, FireAnimation);
    DOREPLIFETIME(AFPS_MultiplayerCharacter, FireSound);

}

float AFPS_MultiplayerCharacter::TakeDamage(float Damage,	struct FDamageEvent const&	DamageEvent, AController*	EventInstigator,	AActor*	DamageCauser)
{
    // Gets the damage done to the dwarf
    float ActualDamage =	Super::TakeDamage(Damage,	DamageEvent,
                                              EventInstigator,	DamageCauser);
    // If the dwarf actually took any damage
    if (ActualDamage >	0.0f)
    {
        // reduce HP by the damage amount
        HP -=	ActualDamage;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), HP));
        
        // If the dwarf has no more health
        if (HP <=	0.0f)
        {
            //	We're	dead
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dead!"));
            dead = true;

            bCanBeDamaged =	false;	//	Don't	allow	further	damage
        }
    }
    return ActualDamage;
}




