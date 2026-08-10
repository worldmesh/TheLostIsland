// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

// Делегаты (Event Dispatchers) для связи с UI. 
// Blueprint персонажа подпишется на них и будет выводить виджет.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionTargetChanged, const FText&, DisplayName, const FText&, ActionText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionTargetCleared);

class AWorldObject;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THELOSTISLAND_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

	// Ссылка на текущий объект взаимодействия
	UPROPERTY()
	AActor* CurrentInteractable = nullptr;

	// Таймер для скрытия информации о WorldObject
	FTimerHandle WorldObjectWidgetTimerHandle;

	void HideInteractionWidget();

public:
	// --- Делегаты ---
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionTargetChanged OnTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionTargetCleared OnTargetCleared;

	// --- Основные методы (перенесены из Character) ---
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* NewInteractable);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClearCurrentInteractable();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentWorldObject(AWorldObject* NewWorldObject);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RefreshInteractionWidget();
};