// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameManager.generated.h"

class AWorldObject;

/**
 * «У объекта сменилось состояние» — единственный канал для всего, что должно
 * реагировать на события, а не на взгляд игрока: строка-уведомление внизу
 * экрана и дневник.
 *
 * Почему через GameManager, а не через InteractionComponent: половина смен
 * состояния к игроку отношения не имеет. Генератор чинится транзишном в тот
 * момент, когда подобран предохранитель, — игрок в это время может смотреть
 * совсем в другую сторону. GameManager видит все смены, компонент игрока — нет.
 *
 * Передаём только сам объект: имя, описание, стейт у него и так читаются
 * BlueprintPure-нодами. Значит сигнатура не поменяется, когда понадобится
 * что-то ещё, и привязки в блюпринте не порвутся.
 *
 * ВНИМАНИЕ: объект живёт не всегда. Пикап после подбора уничтожает себя через
 * DisplayTime — на момент вызова он ещё жив, но указатель НЕ ХРАНИТЬ,
 * забрать тексты сразу и забыть.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectStateChanged, AWorldObject*, Object);

USTRUCT(BlueprintType)
struct FCondition
{
	GENERATED_BODY()

	// Автоподпись строки в Details, например: "BP_Fuse == 1 (Podobran)".
	// Заполняется автоматически при любом изменении в GameManager и по кнопке
	// "Refresh Transition Labels". Руками не редактируется, на геймплей не влияет.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Condition")
	FString Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	AWorldObject* Object = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	int32 RequiredState = 0;
};

USTRUCT(BlueprintType)
struct FAction
{
	GENERATED_BODY()

	// См. комментарий к FCondition::Label.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
	FString Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	AWorldObject* Object = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	int32 NewState = 0;
};

USTRUCT(BlueprintType)
struct FTransition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC")
	FText TransitionName;

	// TitleProperty = "Label" — строки массива показывают человекочитаемую
	// подпись вместо "N members".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC", meta = (TitleProperty = "Label"))
	TArray<FCondition> Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC", meta = (TitleProperty = "Label"))
	TArray<FAction> Actions;
};

UCLASS()
class THELOSTISLAND_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsEvaluatingTransitions = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions", meta = (TitleProperty = "TransitionName"))
	TArray<FTransition> Transitions;

	// Сюда подписываются виджет уведомлений и дневник.
	// В блюпринте: Get Actor Of Class (GameManager) → Bind Event to On Object State Changed.
	UPROPERTY(BlueprintAssignable, Category = "01 GAME LOGIC|Events")
	FOnObjectStateChanged OnObjectStateChanged;

	// Дёргается из AWorldObject::NotifyGameManager() на каждую смену состояния.
	// Руками звать не надо.
	void ReportStateChanged(AWorldObject* Object);

	UFUNCTION(BlueprintCallable)
	void EvaluateTransitions();

	// Пересобирает подписи (Label) у всех Conditions/Actions.
	// CallInEditor рисует кнопку прямо в Details у GameManager — нажимать,
	// если переименовал стейты у объектов и подписи устарели.
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Transitions")
	void RefreshTransitionLabels();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};
