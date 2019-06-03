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

	//Primary Action of the weapon
	void PrimaryAction();

	//Secondary Action of the weapon
	void ActivateSecondaryAction();
	//Secondary Action of the weapon
	void ReleaseSecondaryAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float Range = 1000.f;

	UPROPERTY(EditAnywhere)
	float GrabReach = 300.f;

	UPROPERTY(EditAnywhere)
	float PushForce = 10000.f;

	UPROPERTY(EditAnywhere)
	float PullForce = 20.f;
		
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	FHitResult GetFirstBody(float range) const;
	FVector GetReachLineStart() const;
	FVector GetReachLineEnd(float range) const;

};
