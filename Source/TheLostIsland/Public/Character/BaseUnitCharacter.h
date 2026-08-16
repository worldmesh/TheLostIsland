#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseUnitCharacter.generated.h"

class UAIBrainComponent;
// Без этой строки заголовок не собирался бы сам по себе:
// UBaseMovementComponent используется ниже, а объявлен в другом файле.
class UBaseMovementComponent;

UCLASS()
class THELOSTISLAND_API ABaseUnitCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Конструктор принимающий ObjectInitializer для подмены MovementComponent
	ABaseUnitCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAIBrainComponent> AIBrain;

	FORCEINLINE UAIBrainComponent* GetAIBrain() const { return AIBrain; }

	// Геттер для нашего кастомного движения
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UBaseMovementComponent* GetBaseMovementComponent() const;
};