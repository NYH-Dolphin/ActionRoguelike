// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0F;
}


//Check if the health change is valid, if valid, return true and apply change, else return false
bool USAttributeComponent::ChangeHealth(float delta)
{
	if (Health + delta < 0)
	{
		return false;
	}
	Health += delta;
	return true;
}
