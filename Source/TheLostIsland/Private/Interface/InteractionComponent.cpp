// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/InteractionComponent.h"
#include "Interface/InteractableInterface.h"
#include "WorldObject/WorldObject.h" // Убедитесь, что путь к файлу верный для вашего проекта
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Компоненту взаимодействия не нужен Tick каждый кадр, отключаем для оптимизации
	PrimaryComponentTick.bCanEverTick = false;

	CurrentInteractable = nullptr;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::SetCurrentInteractable(AActor* NewInteractable)
{
	CurrentInteractable = NewInteractable;

	if (!CurrentInteractable)
	{
		return;
	}

	IInteractableInterface* Interactable = Cast<IInteractableInterface>(CurrentInteractable);

	if (Interactable)
	{
		// Вместо вызова виджета напрямую, мы сообщаем всем подписчикам (нашему UI), 
		// что появилась новая цель
		OnTargetChanged.Broadcast(Interactable->GetDisplayName(), Interactable->GetActionText());
	}
}

void UInteractionComponent::ClearCurrentInteractable()
{
	CurrentInteractable = nullptr;
	HideInteractionWidget();
}

void UInteractionComponent::RefreshInteractionWidget()
{
	if (!CurrentInteractable)
	{
		return;
	}

	IInteractableInterface* Interactable = Cast<IInteractableInterface>(CurrentInteractable);

	if (Interactable)
	{
		OnTargetChanged.Broadcast(Interactable->GetDisplayName(), Interactable->GetActionText());
	}
}

void UInteractionComponent::SetCurrentWorldObject(AWorldObject* NewWorldObject)
{
	if (!NewWorldObject)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	// Если предыдущий таймер ещё работает — отменяем его
	World->GetTimerManager().ClearTimer(WorldObjectWidgetTimerHandle);

	// Показываем текст World Object через делегат
	OnTargetChanged.Broadcast(NewWorldObject->GetDisplayName(), NewWorldObject->GetActionText());

	// Запускаем таймер скрытия
	World->GetTimerManager().SetTimer(
		WorldObjectWidgetTimerHandle,
		this,
		&UInteractionComponent::HideInteractionWidget,
		NewWorldObject->GetDisplayTime(),
		false
	);
}

void UInteractionComponent::HideInteractionWidget()
{
	// Сообщаем UI, что пора скрыть виджет
	OnTargetCleared.Broadcast();
}

void UInteractionComponent::Interact()
{
	if (!CurrentInteractable)
	{
		return;
	}

	IInteractableInterface* Interactable = Cast<IInteractableInterface>(CurrentInteractable);

	if (Interactable)
	{
		// 1. Выполняем само действие (подбор, заливка топлива, нажатие рычага)
		Interactable->Interact();

		// 2. СРАЗУ же обновляем виджет, чтобы отобразить новое состояние и текст!
		RefreshInteractionWidget();
	}
}