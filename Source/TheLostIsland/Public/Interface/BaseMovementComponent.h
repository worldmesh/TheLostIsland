#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseMovementComponent.generated.h"

UCLASS()
class THELOSTISLAND_API UBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBaseMovementComponent();

	// Кастомный метод для смены скорости движения
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMaxSpeed(float NewSpeed);
};