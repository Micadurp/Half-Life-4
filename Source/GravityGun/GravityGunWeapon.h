// Nicholas De Young 2019

#pragma once

#include "CoreMinimal.h"
#include "GenericWeapon.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"	
#include "GravityGunWeapon.generated.h"

UCLASS(config=Game)
class GRAVITYGUN_API AGravityGunWeapon : public AGenericWeapon
{
	GENERATED_BODY()

		
public:	
	// Sets default values for this actor's properties
	AGravityGunWeapon();

	// Will shoot an object away, also shoots grabbed object away if one is held
	void PrimaryAction() override;

	// Drags objects towards weapon and grabs them, drops object if an object is grabbed
	void ActivateSecondaryAction() override;
	// Stop pulling objects
	void ReleaseSecondaryAction() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Range for pulling and pushing objects
	UPROPERTY(EditAnywhere)
	float Range = 1000.f;

	// How far away to grab and object, also is distance where object is held
	UPROPERTY(EditAnywhere)
	float GrabReach = 300.f;

	// The Impulse at which the gun shoots away objects
	UPROPERTY(EditAnywhere)
	float PushImpulse = 4000.f;

	// The force at which the gun pulls objects
	UPROPERTY(EditAnywhere)
	float PullForce = 2000.f;
	
	FHitResult GravGunHit;
	bool pulling = false;
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	// Find first physics body within range
	FHitResult GetFirstBody(float _Range) const;
	// Gets starting point for range
	FVector GetReachLineStart() const;
	// Gets end point of range
	FVector GetReachLineEnd(float _Range) const;
};
