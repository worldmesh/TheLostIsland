// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldObject/WorldObject.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "WorldObject/WorldObject.h"

// Sets default values
AWorldObject::AWorldObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWorldObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldObject::OnStateChanged()
{
}

void AWorldObject::NotifyGameManager()
{
    AGameManager* GameManager =
        Cast<AGameManager>(
            UGameplayStatics::GetActorOfClass(
                GetWorld(),
                AGameManager::StaticClass()));

    if (GameManager)
    {
        GameManager->EvaluateTransitions();
    }

    UE_LOG(LogTemp, Warning, TEXT("EvaluateTransitions CALLED"));
}

bool AWorldObject::SetCurrentState(int32 NewState)
{
    if (CurrentState == NewState)
    {
        return false;
    }

    CurrentState = NewState;

    OnStateChanged();

    return true;
}

int32 AWorldObject::GetCurrentState() const
{
    return CurrentState;
}


FText AWorldObject::GetDisplayName() const
{
    return DisplayName;
}

FText AWorldObject::GetDescription() const
{
    if (States.IsValidIndex(CurrentState))
    {
        return States[CurrentState].Description;
    }

    return FText::GetEmpty();
}

FText AWorldObject::GetActionText() const
{
    if (States.IsValidIndex(CurrentState))
    {
        return States[CurrentState].ActionText;
    }

    return FText::GetEmpty();
}

