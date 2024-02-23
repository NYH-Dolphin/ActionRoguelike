// Fill out your copyright notice in the Description page of Project Settings.


#include "SMessageComponent.h"
#include "SMessageInterface.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USMessageComponent::USMessageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void USMessageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USMessageComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void USMessageComponent::SendMessage()
{
	AActor* Owner = GetOwner();

	// get the line trace
	UCameraComponent* CameraComponent = Owner->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent) return;
	FVector Start = Owner->GetActorLocation();
	FVector End = Start + CameraComponent->GetComponentRotation().Vector() * 500.0f;

	// set the check collision channel: world dynamic
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	// set the collision detect shape to be a sphere
	FCollisionShape Shape;
	float Radius = 100.0f;
	Shape.SetSphere(Radius);

	// hit result will be filled in many information
	FHitResult Hit;
	GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjectQueryParams, Shape);

	// check whether the line collide with the actor
	AActor* HitActor = Hit.GetActor();
	if (HitActor)
	{
		// check whether the actor has implemented the interface we want
		if (HitActor->Implements<USMessageInterface>())
		{
			APawn* MyPawn = Cast<APawn>(Owner); // cast to the pawn
			ISMessageInterface::Execute_RecvMessage(HitActor, MyPawn); // use reflection to send the message
		}

		// for debug
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, FColor::Green, false, 2.0f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
	}
}
