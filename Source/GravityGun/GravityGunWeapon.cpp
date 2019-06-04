// Nicholas De Young 2019

#include "GravityGunWeapon.h"
#include "GravityGunProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGravityGunWeapon::AGravityGunWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a gun mesh component
	Weapon_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon_Mesh"));
	Weapon_Mesh->SetOnlyOwnerSee(false);
	Weapon_Mesh->bCastDynamicShadow = false;
	Weapon_Mesh->CastShadow = false;
	Weapon_Mesh->SetupAttachment(RootComponent);

	// Create Muzzle Location
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(Weapon_Mesh);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AGravityGunWeapon::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = this->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *this->GetName())
	}

}

// Called every frame
void AGravityGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Don't crash if Physics Handle isn't added
	if (PhysicsHandle != nullptr) 
	{
		auto GravGunHit = GetFirstBody(Range);
		// If we are pulling and we have hit an actor, give object a force towards the weapon
		if (pulling && GravGunHit.GetActor() != nullptr)
		{
			auto HitComponent = GravGunHit.GetComponent();
			FVector Direction = GetReachLineStart() - HitComponent->GetComponentLocation();
			Direction.Normalize();
			HitComponent->AddForce(Direction * PullForce, NAME_None, true);

			if (FVector::Dist(GetReachLineStart(), HitComponent->GetComponentLocation()) < GrabReach)
			{
				PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetOwner()->GetActorLocation());
				pulling = false;
			}
		}
		else if (PhysicsHandle->GrabbedComponent)
		{
			// If a we have grabbed something hold it infront
			FVector GrabLineEnd = GetReachLineEnd(GrabReach);
			PhysicsHandle->SetTargetLocation(GrabLineEnd);
		}
	}

}

void AGravityGunWeapon::PrimaryAction()
{
	FHitResult Hit = GetFirstBody(Range);
	auto ComponentToPush = Hit.GetComponent();
	auto ActorHit = Hit.GetActor();

	if (ComponentToPush)
	{
		PhysicsHandle->ReleaseComponent();

		FVector Direction = Hit.Location - this->GetActorLocation();
		Direction.Normalize();

		// Add impulse to phys object within range away from weapon
		ComponentToPush->AddImpulse(Direction * PushImpulse, NAME_None, true);

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
	}
}

void AGravityGunWeapon::ActivateSecondaryAction()
{
	// Check if something is grabbed already
	if (PhysicsHandle->GetGrabbedComponent() == nullptr)
	{
		pulling = true;
	}
	else
	{ 
		// If something is grabbed already drop it
		pulling = false;
		PhysicsHandle->ReleaseComponent();
	}
}

void AGravityGunWeapon::ReleaseSecondaryAction()
{
	// If we are pulling an object we need to stop, otherwise nothing
	if (pulling)
	{
		PhysicsHandle->ReleaseComponent();
		pulling = false;
	}
}

FHitResult AGravityGunWeapon::GetFirstBody(float _Range) const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	bool HitSomething = false;

	// Checks if any physics objects are in range
	HitSomething = GetWorld()->LineTraceSingleByObjectType(Hit, GetReachLineStart(), GetReachLineEnd(_Range), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);

	/*if (HitSomething) // Good for debuging
	{
		AActor* ActorHit = Hit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *ActorHit->GetName())
	}*/
	return Hit;
}

FVector AGravityGunWeapon::GetReachLineStart() const
{
	return FP_MuzzleLocation->GetComponentLocation();
}

FVector AGravityGunWeapon::GetReachLineEnd(float _Range) const
{
	return FP_MuzzleLocation->GetComponentLocation() + (FP_MuzzleLocation->GetComponentRotation().Vector() * _Range);
}