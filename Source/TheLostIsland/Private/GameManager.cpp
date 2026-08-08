// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Interactables/InteractableBase.h"


// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
    EvaluateTransitions();
    
	
}

void AGameManager::EvaluateTransitions()
{
    for (const FTransition& Transition : Transitions)
    {
        bool bConditionsMet = true;

        for (const FCondition& Condition : Transition.Conditions)
        {
            if (!Condition.Object)
            {
                bConditionsMet = false;
                break;
            }

            if (Condition.Object->GetCurrentState() != Condition.RequiredState)
            {
                bConditionsMet = false;
                break;
            }
        }

        if (bConditionsMet)
        {
            for (const FAction& Action : Transition.Actions)
            {
                if (Action.Object)
                {
                    Action.Object->SetCurrentState(Action.NewState);
                }
            }
        }
    }
}

