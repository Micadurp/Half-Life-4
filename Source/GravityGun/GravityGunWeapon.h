// Nicholas De Young 2019

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"	
#include "GravityGunWeapon.generated.h"

UCLASS(config=Game)
class GRAVITYGUN_API AGravityGunWeapon : public AActor
{
	GENERATED_BODY()


	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

public:	
	// Sets default values for this actor's properties
	AGravityGunWeapon();

	/** Gun mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Gun_Mesh;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AGravityGunProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimInstance* AnimInstance;

	// Will shoot an object away, also shoots grabbed object away if one is held
	void PrimaryAction();

	// Drags objects towards weapon and grabs them, drops object if an object is grabbed
	void ActivateSecondaryAction();
	// Stop pulling objects
	void ReleaseSecondaryAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetAnimInstance(UAnimInstance* _AnimInstance);

private:
	// Range for pulling and pushing objects
	UPROPERTY(EditAnywhere)
	float Range = 1000.f;

	// How far away to grab and object, also is distance where object is held
	UPROPERTY(EditAnywhere)
	float GrabReach = 300.f;

	// The Impulse at which the gun shoots away objects
	UPROPERTY(EditAnywhere)
	float PushImpulse = 10000.f;

	// The force at which the gun pulls objects
	UPROPERTY(EditAnywhere)
	float PullForce = 1000.f;
	
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
