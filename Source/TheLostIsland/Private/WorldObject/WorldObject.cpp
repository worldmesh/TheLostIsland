// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldObject/WorldObject.h"
#include "GameManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

AWorldObject::AWorldObject()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AWorldObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldObject::OnStateChanged()
{
    ApplyStateEffects();

    NotifyGameManager();
}

void AWorldObject::NotifyGameManager()
{
    UE_LOG(LogTemp, Verbose,
        TEXT("NotifyGameManager: %s State=%d"),
        *GetName(),
        CurrentState);

    AGameManager* GameManager =
        Cast<AGameManager>(
            UGameplayStatics::GetActorOfClass(
                GetWorld(),
                AGameManager::StaticClass()));

    if (GameManager)
    {
        GameManager->ReportStateChanged(this);

        GameManager->EvaluateTransitions();
    }

    UE_LOG(LogTemp, Verbose, TEXT("EvaluateTransitions CALLED"));
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

int32 AWorldObject::GetStateCount() const
{
    return States.Num();
}

FString AWorldObject::GetStateName(int32 StateIndex) const
{
    if (!States.IsValidIndex(StateIndex))
    {
        return FString();
    }

    return States[StateIndex].StateName;
}

FText AWorldObject::GetActionTextForState(int32 StateIndex) const
{
    if (!States.IsValidIndex(StateIndex))
    {
        return FText::GetEmpty();
    }

    return States[StateIndex].ActionText;
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

float AWorldObject::GetDisplayTime() const
{
    return DisplayTime;
}

float AWorldObject::GetMessageDelay() const
{
    return GetMessageDelayForState(CurrentState);
}

float AWorldObject::GetMessageDelayForState(int32 StateIndex) const
{
    // Невалидный индекс — возвращаем ноль, а не что-то «на всякий случай».
    // Ноль означает «показать сразу», то есть старое поведение: даже если
    // объект настроен криво, сообщение не потеряется, просто не будет паузы.
    if (!States.IsValidIndex(StateIndex))
    {
        return 0.f;
    }

    return States[StateIndex].MessageDelay;
}

void AWorldObject::ApplyStateEffects()
{
	if (!States.IsValidIndex(CurrentState))
	{
		return;
	}

	const FStateEffects& Effects = States[CurrentState].Effects;

	// Static Mesh
	if (Effects.StaticMesh)
	{
		UStaticMeshComponent* MeshComponent =
			FindComponentByClass<UStaticMeshComponent>();

		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(Effects.StaticMesh);
		}
	}

	// Material
	if (Effects.Material)
	{
		UStaticMeshComponent* MeshComponent =
			FindComponentByClass<UStaticMeshComponent>();

		if (MeshComponent)
		{
			MeshComponent->SetMaterial(
				Effects.MaterialSlot,
				Effects.Material
			);
		}
	}

	// Sound
	if (Effects.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			Effects.Sound,
			GetActorLocation()
		);
	}

	// Animation
	if (Effects.Animation)
	{
		USkeletalMeshComponent* SkeletalMeshComponent =
			FindComponentByClass<USkeletalMeshComponent>();

		if (SkeletalMeshComponent)
		{
			SkeletalMeshComponent->PlayAnimation(
				Effects.Animation,
				false
			);
		}
	}

	// Niagara
	if (Effects.Niagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			Effects.Niagara,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Sequencer
	if (Effects.Sequencer)
	{
		FMovieSceneSequencePlaybackSettings PlaybackSettings;

		ALevelSequenceActor* SequenceActor = nullptr;

		ULevelSequencePlayer* SequencePlayer =
			ULevelSequencePlayer::CreateLevelSequencePlayer(
				GetWorld(),
				Effects.Sequencer,
				PlaybackSettings,
				SequenceActor
			);

		if (SequencePlayer)
		{
			SequencePlayer->Play();
		}
	}
}
