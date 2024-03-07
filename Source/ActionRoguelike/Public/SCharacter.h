// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMessageComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "SCharacter.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public: // Constructor
	// Sets default values for this character's properties
	ASCharacter();

protected: // Sub-Component
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	USMessageComponent* MessageComponent;

protected: // Attack Related

	UPROPERTY(EditAnywhere, Category="Attack")
	float FAttackPeriod;

	UPROPERTY(EditAnywhere, Category="Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> MagicProjectileClass; // assign it in blueprint

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> BlackHoleProjectileClass; // assign it in blueprint

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> TeleportProjectileClass;
	void PrimaryAttack();
	void SkillAttack();
	void Teleport();
	void SpawnMagicProjectile();
	void SpawnBlackHoleProjectile();
	void SpawnTeleportProjectile();
	void SpawnProjectile(UClass* Class);

	void RefreshAttack();

private:
	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_AttackPeriod;

	bool bCanAttack;

protected: // Movement Related
	UPROPERTY(EditAnywhere)
	float FSprintScale;

	// Character Movement Definition
	void MoveForward(float Value);

	void MoveRight(float Value);

	void JumpPerformed();

	void JumpCanceled();

	void SprintPerformed();

	void SprintCanceled();

	void Interact();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
