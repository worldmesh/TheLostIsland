// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/BaseAIController.h"
#include "Interface/AIBrainComponent.h"

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) return;

	// Ищем компонент мозга у захваченного персонажа
	if (UAIBrainComponent* Brain = InPawn->FindComponentByClass<UAIBrainComponent>())
	{
		if (Brain->BehaviorTreeAsset)
		{
			// Запускаем Behavior Tree, привязав к нему его Blackboard
			RunBehaviorTree(Brain->BehaviorTreeAsset);
		}
	}
}