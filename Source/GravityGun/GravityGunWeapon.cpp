// Nicholas De Young 2019

#include "GravityGunWeapon.h"
#include "GravityGunProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"


// Sets default values
AGravityGunWeapon::AGravityGunWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a gun mesh component
	Gun_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun_Mesh"));
	Gun_Mesh->SetOnlyOwnerSee(false);
	Gun_Mesh->bCastDynamicShadow = false;
	Gun_Mesh->CastShadow = false;

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(Gun_Mesh);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

}

// Called when the game starts or when spawned
void AGravityGunWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGravityGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGravityGunWeapon::PrimaryAction()
{
	// Add impulse to phys object within range away from weapon
}

void AGravityGunWeapon::SecondaryAction()
{
	// Add force to phys object within range towards weapon and grab object if close enough
}

/*void AGravityGunWeapon::PrimaryAction()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AGravityGunProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}*/
