// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Declare the components
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent); // RootComponent: the first component of the hierarchy

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent); // attached the camera to the springarm

	MessageComponent = CreateDefaultSubobject<USMessageComponent>("MessageComponent");

	// Movement details setting
	SpringArmComponent->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Sprint setting
	FSprintScale = 1.3f;

	FAttackPeriod = 0.5f;
	bCanAttack = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintPerformed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintCanceled);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::Interact);
}

void ASCharacter::RefreshAttack()
{
	bCanAttack = true;
}

// TODO this function will be modified later
void ASCharacter::PrimaryAttack()
{
	// check whether the player can attack
	if (!bCanAttack) return;
	bCanAttack = false;

	// play the attack animation
	PlayAnimMontage(AttackAnim);

	// set the rotation of the character to be the same as the control rotation
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	SetActorRotation(ControlRotation);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::SpawnProjectile, 0.2f);

	// delay a certain period of time to execute the spawn projectile
	// call spawn projectile in 0.2 sec
	GetWorldTimerManager().SetTimer(TimerHandle_AttackPeriod, this, &ASCharacter::RefreshAttack, FAttackPeriod);
}

void ASCharacter::SpawnProjectile()
{
	// spawn transform matrix
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f; // move horizontally
	ControlRotation.Roll = 0.0f;
	FTransform SpawnTM = FTransform(ControlRotation, HandLocation);

	// specify the spawn rules
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// spawn
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}


void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	AddMovementInput(ControlRotation.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	AddMovementInput(FQuat(ControlRotation).GetRightVector(), Value);
}

void ASCharacter::JumpPerformed()
{
	Jump();
}

void ASCharacter::JumpCanceled()
{
	StopJumping();
}

void ASCharacter::SprintPerformed()
{
	GetCharacterMovement()->MaxWalkSpeed *= FSprintScale;
}

void ASCharacter::SprintCanceled()
{
	GetCharacterMovement()->MaxWalkSpeed /= FSprintScale;
}


void ASCharacter::Interact()
{
	if (MessageComponent)
	{
		MessageComponent->SendMessage();
	}
}
