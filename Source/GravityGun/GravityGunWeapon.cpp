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
	Gun_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun_Mesh"));
	Gun_Mesh->SetOnlyOwnerSee(false);
	Gun_Mesh->bCastDynamicShadow = false;
	Gun_Mesh->CastShadow = false;

	// Create Muzzle Location
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(Gun_Mesh);
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


	if (PhysicsHandle != nullptr) // Don't crash if Physics Handle isn't added
	{
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
				//PhysicsHandle->SetTargetLocation(GrabLineEnd);
			}
		}
		else if (PhysicsHandle->GrabbedComponent)
		{
			FVector GrabLineEnd = GetReachLineEnd(GrabReach);
			PhysicsHandle->SetTargetLocation(GrabLineEnd);
		}
	}

}

// Will shoot an object away, also shoots grabbed object away if one is held
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

		ComponentToPush->AddImpulse(Direction * PushForce, NAME_None, true);
		// Add impulse to phys object within range away from weapon
	}
}

// Drags objects towards weapon and grabs them, drops object if an object is grabbed
void AGravityGunWeapon::ActivateSecondaryAction()
{
	// Check if something is grabbed already
	if (PhysicsHandle->GetGrabbedComponent() == nullptr)
	{
		pulling = true;
		GravGunHit = GetFirstBody(Range);
		auto ComponentToGrab = GravGunHit.GetComponent();
		auto ActorHit = GravGunHit.GetActor();

		if (ActorHit)
		{
			pulling = true;
		}
	}
	else // If something is grabbed already drop it
	{
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

// Shoots line trace forward and will return a hitresult if it hit a physics object
FHitResult AGravityGunWeapon::GetFirstBody(float range) const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	bool HitSomething = false;
	HitSomething = GetWorld()->LineTraceSingleByObjectType(Hit, GetReachLineStart(), GetReachLineEnd(range), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);

	AActor* ActorHit = Hit.GetActor();

	if (HitSomething)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *ActorHit->GetName())
	}
	return Hit;
}

FVector AGravityGunWeapon::GetReachLineStart() const // TODO should this be from the view of the gun?
{
	return FP_MuzzleLocation->GetComponentLocation();
}

FVector AGravityGunWeapon::GetReachLineEnd(float range) const
{
	return FP_MuzzleLocation->GetComponentLocation() + (FP_MuzzleLocation->GetComponentRotation().Vector() * range);
}