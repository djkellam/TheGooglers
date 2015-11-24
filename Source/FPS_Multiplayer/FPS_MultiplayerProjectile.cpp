// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FPS_Multiplayer.h"
#include "FPS_MultiplayerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FPS_MultiplayerCharacter.h"
#include "Engine.h"

AFPS_MultiplayerProjectile::AFPS_MultiplayerProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPS_MultiplayerProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 1 seconds by default
	InitialLifeSpan = 1.0f;
    
    // Allows projectiles to be sent to server
    bReplicates = true;
    
    damage = 34;
    
}

void AFPS_MultiplayerProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if(Role == ROLE_Authority)
    {
        ServerOnHit(OtherActor, OtherComp, NormalImpulse, Hit);
    }
}

void AFPS_MultiplayerProjectile::ServerOnHit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AFPS_MultiplayerCharacter* TestCharacter = Cast<AFPS_MultiplayerCharacter>(OtherActor);
    
    if(TestCharacter)
    {
        TestCharacter->TakeDamage(damage, FDamageEvent(),	GetInstigatorController(),	this);
    }
    // Only add impulse and destroy projectile if we hit a physics
    if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
        
        Destroy();
    }

}

bool AFPS_MultiplayerProjectile::ServerOnHit_Validate(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    return true;
}

/*void AFPS_MultiplayerProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    //DOREPLIFETIME(AFPS_MultiplayerProjectile, );
}*/






