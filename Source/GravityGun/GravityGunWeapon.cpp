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

	FVector LineTraceEnd = GetReachLineEnd(GrabReach);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
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

		UE_LOG(LogTemp, Warning, TEXT("direction: %s"), *Direction.ToString())
		UE_LOG(LogTemp, Warning, TEXT("Component: %s"), *ComponentToPush->GetName())
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
		FHitResult DragHit = GetFirstBody(Range);

		// Start dragging object towards player, grab if within range
		// Maybe check if the Draghit is within Grabreach and then grab it?

		FHitResult GrabHit = GetFirstBody(GrabReach);
		auto ComponentToGrab = GrabHit.GetComponent();
		auto ActorHit = GrabHit.GetActor();

		if (ActorHit)
		{
			PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);
		}
		// Add force to phys object within range towards weapon and grab object if close enough
	}
	else // If something is grabbed already drop it
	{
		PhysicsHandle->ReleaseComponent();
	}
}

void AGravityGunWeapon::ReleaseSecondaryAction()
{
	// Should stop the dragging if we are dragging
}

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
	FVector ActorPosition;
	FRotator ActorRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ActorPosition, ActorRotation);

	return ActorPosition;
}

FVector AGravityGunWeapon::GetReachLineEnd(float range) const
{
	FVector ActorPosition;
	FRotator ActorRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ActorPosition, ActorRotation);

	return ActorPosition + (ActorRotation.Vector() * range);
}