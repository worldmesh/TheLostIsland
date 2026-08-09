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
    ApplyStateEffects();

    NotifyGameManager();
}

void AWorldObject::NotifyGameManager()
{
    UE_LOG(LogTemp, Warning,
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

float AWorldObject::GetDisplayTime() const
{
    return DisplayTime;
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
