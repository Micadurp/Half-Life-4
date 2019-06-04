// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GravityGunCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGravityGunCharacter

AGravityGunCharacter::AGravityGunCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

void AGravityGunCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Attaching gun model to arm mesh
	if (GenericWeaponBlueprint != NULL)
	{
		GenericWeapon = GetWorld()->SpawnActor<AGenericWeapon>(GenericWeaponBlueprint);
		GenericWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		GenericWeapon->SetAnimInstance(Mesh1P->GetAnimInstance());
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGravityGunCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind Weapon action events events
	PlayerInputComponent->BindAction("WeaponPrimaryAction", IE_Pressed, this, &AGravityGunCharacter::ActivatePrimaryAction);
	PlayerInputComponent->BindAction("WeaponSecondaryAction", IE_Pressed, this, &AGravityGunCharacter::ActivateSecondaryAction);
	PlayerInputComponent->BindAction("WeaponSecondaryAction", IE_Released, this, &AGravityGunCharacter::ReleaseSecondaryAction);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGravityGunCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityGunCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGravityGunCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGravityGunCharacter::LookUpAtRate);


	PlayerInputComponent->BindAction("GrabWeapon", IE_Pressed, this, &AGravityGunCharacter::PickupWeapon);
	PlayerInputComponent->BindAction("TossWeapon", IE_Pressed, this, &AGravityGunCharacter::ThrowWeapon);
}

// Wanted to do this but would need more time to figure out/research how
void AGravityGunCharacter::PickupWeapon()
{ 
	return;
	/*TArray<AActor*> Weapons;
	GetOwner()->GetOverlappingActors(Weapons, TSubclassOf<AGenericWeapon>());

	for (AActor* Weapon : Weapons)
	{
		//((AGenericWeapon*) Weapon)->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		//((AGenericWeapon*) Weapon)->SetAnimInstance(Mesh1P->GetAnimInstance());
	}*/
}

// Wanted to do this but would need more time to figure out/research how
void AGravityGunCharacter::ThrowWeapon()
{

	return;
	//GenericWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	//GenericWeapon->SetAnimInstance(nullptr);
}

#pragma region ActivatePrimaryAction

/* Three functions that propagate the action over the network */
void AGravityGunCharacter::ActivatePrimaryAction()
{
	GenericWeapon->PrimaryAction();

	if (Role < ROLE_Authority)
	{
		ServerActivatePrimaryAction();
	}
}

bool AGravityGunCharacter::ServerActivatePrimaryAction_Validate()
{
	return true;
}

void AGravityGunCharacter::ServerActivatePrimaryAction_Implementation()
{
	ActivatePrimaryAction();
}

#pragma endregion

#pragma region ActivateSecondaryAction

/* Three functions that propagate the action over the network */
void AGravityGunCharacter::ActivateSecondaryAction()
{
	GenericWeapon->ActivateSecondaryAction();

	if (Role < ROLE_Authority)
	{
		ServerActivateSecondaryAction();
	}
}

bool AGravityGunCharacter::ServerActivateSecondaryAction_Validate()
{
	return true;
}

void AGravityGunCharacter::ServerActivateSecondaryAction_Implementation()
{
	ActivateSecondaryAction();
}

#pragma endregion

#pragma region ActivateSecondaryAction

/* Three functions that propagate the action over the network */
void AGravityGunCharacter::ReleaseSecondaryAction()
{
	GenericWeapon->ReleaseSecondaryAction();

	if (Role < ROLE_Authority)
	{
		ServerReleaseSecondaryAction();
	}
}

bool AGravityGunCharacter::ServerReleaseSecondaryAction_Validate()
{
	return true;
}

void AGravityGunCharacter::ServerReleaseSecondaryAction_Implementation()
{
	ReleaseSecondaryAction();
}

#pragma endregion

void AGravityGunCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGravityGunCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGravityGunCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGravityGunCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
