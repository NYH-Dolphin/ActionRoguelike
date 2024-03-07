// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "DrawDebugHelpers.h"
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

	// Debug for direction
	float ArrowLength = 60.0f;
	float Offset = 200.0f;
	FVector Start = GetActorLocation() + GetActorRightVector() * Offset;
	FVector EndActor = Start + ArrowLength * GetActorRotation().Vector();
	FVector EndController = Start + ArrowLength * GetControlRotation().Vector();
	DrawDebugDirectionalArrow(GetWorld(), Start, EndActor, 10.0f, FColor::Yellow, false, -1, 0, 3.0f);
	DrawDebugDirectionalArrow(GetWorld(), Start, EndController, 10.0f, FColor::Green, false, -1, 1, 3.0f);
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
	PlayerInputComponent->BindAction("SkillAttack", IE_Pressed, this, &ASCharacter::SkillAttack);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &ASCharacter::Teleport);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::Interact);
}

void ASCharacter::RefreshAttack()
{
	bCanAttack = true;
}

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

	// delay a certain period of time to execute the spawn projectile
	// call spawn projectile in 0.2 sec
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::SpawnMagicProjectile, 0.2f);
	GetWorldTimerManager().SetTimer(TimerHandle_AttackPeriod, this, &ASCharacter::RefreshAttack, FAttackPeriod);
}

void ASCharacter::SkillAttack()
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

	// delay a certain period of time to execute the spawn projectile
	// call spawn projectile in 0.25 sec
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::SpawnBlackHoleProjectile, 0.25f);
	GetWorldTimerManager().SetTimer(TimerHandle_AttackPeriod, this, &ASCharacter::RefreshAttack, FAttackPeriod);
}

void ASCharacter::Teleport()
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

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::SpawnTeleportProjectile, 0.2f);
	GetWorldTimerManager().SetTimer(TimerHandle_AttackPeriod, this, &ASCharacter::RefreshAttack, FAttackPeriod);
}

void ASCharacter::SpawnMagicProjectile()
{
	SpawnProjectile(MagicProjectileClass);
}

void ASCharacter::SpawnBlackHoleProjectile()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::SpawnTeleportProjectile()
{
	SpawnProjectile(TeleportProjectileClass);
}


void ASCharacter::SpawnProjectile(UClass* Class)
{
	// get the line trace from the camera, really long, to detect the possible 
	float Length = 100000.0f;
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + CameraComponent->GetComponentRotation().Vector() * Length;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	// hit result will be filled in many information
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);
	// check whether the line trace hit with an actor, if true, get the hit location, else directly using the line trace
	AActor* HitActor = Hit.GetActor();
	FVector HitLocation = HitActor ? Hit.Location : End;
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	// spawn position and rotation, to construct the rotation matrix
	// spawn forward a little
	FVector SpawnLocation = HandLocation + FQuat(GetMesh()->GetSocketRotation("Muzzle_01")).GetForwardVector() * 20.0f;
	FRotator SpawnRotation = FRotationMatrix::MakeFromX((HitLocation - HandLocation).GetSafeNormal()).Rotator();
	FTransform SpawnTM = FTransform(SpawnRotation, SpawnLocation);
	// specify the spawn rules
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	// spawn
	GetWorld()->SpawnActor<AActor>(Class, SpawnTM, SpawnParams);
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
