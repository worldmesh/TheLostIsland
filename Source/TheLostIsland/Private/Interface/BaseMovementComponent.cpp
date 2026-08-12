#include "Interface/BaseMovementComponent.h"

UBaseMovementComponent::UBaseMovementComponent()
{
	// Конструктор компонента движения
}

void UBaseMovementComponent::SetMaxSpeed(float NewSpeed)
{
	MaxWalkSpeed = NewSpeed;
}