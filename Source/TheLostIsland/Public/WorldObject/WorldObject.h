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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString StateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionText;

	// Пауза перед тем, как показать текст в диалоговом окне, в секундах.
	// Отсчитывается от момента входа в состояние.
	//
	// Зачем: эффекты состояния (звук, анимация, Niagara, секвенсер) стартуют
	// одновременно со сменой стейта, и текст, выскочивший в ту же миллисекунду,
	// обгоняет событие — читается как подпись к тому, чего ещё не произошло.
	// Задержка даёт событию отыграть, а тексту — стать его следствием.
	//
	// Значение подбирать под самый длинный эффект СТЕЙТА, а не объекта:
	// у канистры это короткий звук (~0.4), у генератора — секвенсер (2-3).
	// Ноль = старое поведение, текст мгновенно. Дефолт ноль намеренно:
	// пока значение не выставлено руками, объект ведёт себя как раньше.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
		meta = (ClampMin = "0.0", UIMax = "5.0"))
	float MessageDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FStateEffects Effects;
};


UCLASS()
class THELOSTISLAND_API AWorldObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldObject();

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC", meta = (TitleProperty = "StateName"))
	TArray<FInteractionState> States;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	int32 CurrentState = 0;
	virtual void OnStateChanged();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	bool bCanInteract = true;
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

	// Сколько времени висит сообщение. ВНИМАНИЕ: это поле ОБЪЕКТА, а не стейта —
	// одно на все состояния. MessageDelay, наоборот, лежит внутри
	// FInteractionState, и у каждого состояния он свой.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float DisplayTime = 1.5f;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetDisplayTime() const;

	// Задержка перед показом текста для ТЕКУЩЕГО состояния.
	// Это то, что нужно в 99% случаев — брать в WBP_Dialogue.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetMessageDelay() const;

	// То же для произвольного состояния. Нужно, если понадобится заглянуть
	// вперёд — например, посчитать длину цепочки переходов.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetMessageDelayForState(int32 StateIndex) const;

	UFUNCTION(BlueprintCallable)
	bool SetCurrentState(int32 NewState);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentState() const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	int32 GetStateCount() const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FString GetStateName(int32 StateIndex) const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FText GetActionTextForState(int32 StateIndex) const;

	void ApplyStateEffects();
};
