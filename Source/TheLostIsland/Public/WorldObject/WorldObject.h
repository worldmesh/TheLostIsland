// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WorldObject.generated.h"

class UStaticMesh;
class USoundBase;
class UAnimationAsset;
class ULevelSequence;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FStateEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	int32 MaterialSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UAnimationAsset* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	ULevelSequence* Sequencer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* Niagara = nullptr;
};

USTRUCT(BlueprintType)
struct FInteractionState
{
	GENERATED_BODY()

	// Название стейта только для удобства в редакторе (на геймплей не влияет).
	// Например: "Осмотреть", "Заправлена", "Уехала".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString StateName;

	// Description shown in interaction window.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Description;

	// Action shown to the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FStateEffects Effects;
};


UCLASS()
class THELOSTISLAND_API AWorldObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldObject();

protected:
	
	virtual void BeginPlay() override;
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	FText DisplayName;
	// TitleProperty заставляет каждую строку массива в Details показывать
	// StateName вместо "Index [0]" — сразу видно, какой стейт за что отвечает.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC", meta = (TitleProperty = "StateName"))
	TArray<FInteractionState> States;
	// Current state index.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	int32 CurrentState = 0;
	virtual void OnStateChanged();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	bool bCanInteract = true;
	// True until the first successful interaction.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	bool bFirstInteraction = true;

	void NotifyGameManager();


public:	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetDescription() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetActionText() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float DisplayTime = 1.5f;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetDisplayTime() const;

	UFUNCTION(BlueprintCallable)
	bool SetCurrentState(int32 NewState);

	// Returns the current interaction state.
	UFUNCTION(BlueprintPure)
	int32 GetCurrentState() const;

	// Сколько всего стейтов у ЭТОГО объекта (у инстанса на уровне, а не у
	// блюпринт-класса). Нужно, чтобы GameManager мог проверить, что номер
	// стейта в Condition/Action вообще существует.
	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	int32 GetStateCount() const;

	// Читаемое имя стейта по индексу (поле StateName). Пустая строка, если
	// индекс невалиден или имя не заполнено.
	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FString GetStateName(int32 StateIndex) const;

	// ActionText конкретного стейта по индексу (а не текущего, как GetActionText).
	// Пустой FText, если индекс невалиден.
	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FText GetActionTextForState(int32 StateIndex) const;

	void ApplyStateEffects();
};
