// BaseUnitCharacter.cpp
#include "Character/BaseUnitCharacter.h"
#include "Interface/AIBrainComponent.h"
#include "Interface/BaseAIController.h"
#include "Interface/BaseMovementComponent.h"

ABaseUnitCharacter::ABaseUnitCharacter(const FObjectInitializer& ObjectInitializer)
// Переопределяем стандартный CharacterMovementComponent на наш UBaseMovementComponent!
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	AIBrain = CreateDefaultSubobject<UAIBrainComponent>(TEXT("AIBrain"));

	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UBaseMovementComponent* ABaseUnitCharacter::GetBaseMovementComponent() const
{
	return Cast<UBaseMovementComponent>(GetCharacterMovement());
}