// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Interactables/InteractableBase.h"
#include "WorldObject/WorldObject.h"


namespace
{
    // Собирает подпись вида "Канистра == Подобрана [1]".
    // Имя объекта берём из DisplayName (то, что видит игрок), а не из имени
    // актора на уровне; название состояния — из ActionText этого стейта.
    // Если что-то из этого не заполнено — аккуратно откатываемся на
    // техническое имя актора / StateName / просто номер.
    // Arrow: "==" для Condition (проверяем), "->" для Action (выставляем).
    // Если индекс стейта выходит за размер массива States у объекта —
    // подпись сразу об этом кричит. Это ровно тот баг, на который мы уже
    // трижды наступали (у инстанса на уровне States короче, чем в блюпринте).
    FString MakeSlotLabel(const AWorldObject* Object, int32 StateIndex, const TCHAR* Arrow)
    {
        if (!Object)
        {
            return FString(TEXT("--- object not set ---"));
        }

        // 1. Как называется объект.
        FString ObjectName = Object->GetDisplayName().ToString();

        if (ObjectName.IsEmpty())
        {
#if WITH_EDITOR
            ObjectName = Object->GetActorLabel();
#else
            ObjectName = Object->GetName();
#endif
        }

        // Arrow может быть пустой строкой ("") — тогда стрелку/значок вообще
        // не показываем, просто "Имя СостояниеТекст [Индекс]".
        const FString ArrowPart = (Arrow && *Arrow) ? FString::Printf(TEXT("%s "), Arrow) : FString();

        // 2. Существует ли вообще такой стейт у этого инстанса.
        const int32 StateCount = Object->GetStateCount();

        if (StateIndex < 0 || StateIndex >= StateCount)
        {
            return FString::Printf(TEXT("%s %s[%d]   <<< INVALID STATE (states: %d)"),
                *ObjectName, *ArrowPart, StateIndex, StateCount);
        }

        // 3. Как называется состояние: сначала ActionText, потом StateName.
        FString StateLabel = Object->GetActionTextForState(StateIndex).ToString();

        if (StateLabel.IsEmpty())
        {
            StateLabel = Object->GetStateName(StateIndex);
        }

        if (StateLabel.IsEmpty())
        {
            return FString::Printf(TEXT("%s %s[%d]"), *ObjectName, *ArrowPart, StateIndex);
        }

        return FString::Printf(TEXT("%s %s%s [%d]"), *ObjectName, *ArrowPart, *StateLabel, StateIndex);
    }
}


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

void AGameManager::RefreshTransitionLabels()
{
    for (FTransition& Transition : Transitions)
    {
        for (FCondition& Condition : Transition.Conditions)
        {
            // Для Condition стрелку убрали — визуально и так понятно, что это
            // проверка состояния, а "==" только загромождал строку.
            Condition.Label = MakeSlotLabel(Condition.Object, Condition.RequiredState, TEXT(""));
        }

        for (FAction& Action : Transition.Actions)
        {
            Action.Label = MakeSlotLabel(Action.Object, Action.NewState, TEXT("->"));
        }
    }
}

#if WITH_EDITOR
void AGameManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Любая правка в Details (выбрали объект, поменяли номер стейта,
    // добавили строку) — сразу пересобираем подписи.
    RefreshTransitionLabels();
}
#endif

void AGameManager::EvaluateTransitions()
{
    if (bIsEvaluatingTransitions)
    {
        return;
    }

    bIsEvaluatingTransitions = true;

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
                if (!Action.Object)
                {
                    continue;
                }

                // Тот самый молчаливый баг, на который уже наступали: если у
                // инстанса на уровне массив States короче, чем в блюпринте,
                // SetCurrentState выставит несуществующий индекс, ApplyStateEffects
                // молча ничего не сделает, а тексты станут пустыми — и ни одной
                // ошибки в логе. Теперь ругаемся явно и состояние не портим.
                if (Action.NewState < 0 || Action.NewState >= Action.Object->GetStateCount())
                {
                    UE_LOG(LogTemp, Error,
                        TEXT("GameManager: транзишн '%s' пытается выставить объекту '%s' стейт %d, ")
                        TEXT("а у него всего %d стейтов. Действие пропущено. ")
                        TEXT("Проверь массив States у инстанса на уровне (Reset to Class Default)."),
                        *Transition.TransitionName.ToString(),
                        *Action.Object->GetName(),
                        Action.NewState,
                        Action.Object->GetStateCount());
                    continue;
                }

                Action.Object->SetCurrentState(Action.NewState);
            }
        }
    }
    bIsEvaluatingTransitions = false;
}

