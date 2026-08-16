// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AActor;
class AWorldObject;

/**
 * Всё, что виджет может захотеть показать про объект — одной посылкой.
 *
 * Почему структурой, а не списком параметров у делегата: КАЖДОЕ изменение
 * сигнатуры делегата рвёт привязку в блюпринте — ноду события приходится
 * удалять и создавать заново, теряя всё, что к ней было прицеплено.
 * С структурой сигнатура больше не меняется никогда: понадобилось новое поле —
 * дописал сюда строчку, старые привязки продолжают работать как ни в чём
 * не бывало, а в блюпринте у пина Info просто появляется новый выход в Break.
 *
 * Разбирать в блюпринте: правый клик по пину Info → Split Struct Pin,
 * либо нода Break Interaction Info.
 */
USTRUCT(BlueprintType)
struct FInteractionInfo
{
	GENERATED_BODY()

	// Имя объекта для игрока — в заголовок плашки. "Канистра", "Маяк".
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FText DisplayName;

	// Что игрок сделает — к клавише [E]. "Взять", "Заправить", "Осмотреть".
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FText ActionText;

	// Описание текущего состояния — строка под заголовком.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FText Description;

	// Техническое имя стейта (поле StateName). Игроку не показывать — оно
	// для редактора; годится для отладки и для выбора иконки по имени стейта.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FString StateName;

	// Номер текущего стейта. Удобно гнать в Switch on Int, если для разных
	// состояний нужна разная вёрстка. INDEX_NONE (-1), если объект не WorldObject.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	int32 StateIndex = INDEX_NONE;

	// Сколько секунд висит сообщение. Осмысленно для диалогового окна
	// (там по этому времени работает таймер скрытия); для плашки интеракта
	// просто справочное значение — она живёт, пока игрок смотрит.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	float DisplayTime = 0.f;

	// Сам объект — если виджету понадобится что-то ещё, чего тут нет.
	// Проверять на Is Valid перед использованием: пикап мог уже уничтожиться.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* Actor = nullptr;
};

// Делегаты (Event Dispatchers) для связи с UI.
// Blueprint персонажа подпишется на них и будет выводить виджет.
//
// ВАЖНО: делегатов две пары, и путать их нельзя.
//
// 1) OnTargetChanged / OnTargetCleared — ТОЛЬКО трейс от камеры.
//    Это плашка интеракта: висит, пока игрок смотрит на объект, привязана
//    к точке в мире, гаснет сразу как отвёл взгляд.
//
// 2) OnWorldObjectMessage / OnWorldObjectMessageCleared — ТОЛЬКО EventObject.
//    Это диалоговое окно: появляется при входе в триггер, стоит на месте,
//    гаснет само по таймеру DisplayTime.
//
// Раньше обе системы вещали в одну пару, и виджет не мог понять, кто его
// дёрнул: текст события затирал текст плашки, а "отвёл взгляд" обрывал
// диалог на полуслове. Не сливать обратно.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChanged, const FInteractionInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionTargetCleared);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldObjectMessage, const FInteractionInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWorldObjectMessageCleared);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THELOSTISLAND_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// Ссылка на текущий объект взаимодействия
	UPROPERTY()
	AActor* CurrentInteractable = nullptr;

	// Показан ли сейчас виджет. Нужен отдельно от CurrentInteractable, потому что
	// CurrentInteractable — UPROPERTY: когда актор уничтожается (подобрали пикап),
	// движок сам обнуляет указатель, и сравнение "новая цель == текущая" начинает
	// давать true при обеих nullptr — виджет тогда не прятался и висел на экране.
	bool bTargetWidgetShown = false;

	// Точка, куда упёрся трейс на последнем кадре — в мировых координатах.
	// Именно к ней виджет привязывает ромб-якорь и выноску: центр актора
	// не подходит, у крупных объектов (дом, маяк, лодка) он далеко от
	// того места, куда реально смотрит игрок.
	FVector LastImpactPoint = FVector::ZeroVector;

	// Таймер для скрытия информации о WorldObject
	FTimerHandle WorldObjectWidgetTimerHandle;

	// Гасит ПЛАШКУ ИНТЕРАКТА (то, что показал трейс).
	void HideInteractionWidget();

	// Гасит ДИАЛОГОВОЕ ОКНО (то, что показал EventObject по таймеру).
	// Отдельный метод, а не общий с HideInteractionWidget: иначе таймер события
	// гасил бы заодно и плашку интеракта, и наоборот.
	void HideWorldObjectWidget();

	// Собирает FInteractionInfo по актору. Единственное место, где данные
	// вытаскиваются из объекта — если появится новое поле, дописывать сюда,
	// и оно само доедет до обоих делегатов и до GetCurrentInteractionInfo().
	FInteractionInfo MakeInteractionInfo(AActor* Target) const;

	// --- Поиск цели трейсом ---

	// Каждый тик стреляем лучом из камеры и решаем, на что смотрит игрок.
	void UpdateInteractionTrace();

	// true  — цель выбирается трейсом от камеры (новая схема).
	// false — цель выбирается пересечением с InteractionBox (старая схема).
	// Оставлено переключателем, чтобы можно было быстро сравнить/откатиться.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	bool bUseLineTrace = true;

	// Длина луча от камеры. В третьем лице камера висит позади персонажа,
	// поэтому луч должен быть заметно длиннее, чем дистанция интеракта.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float TraceDistance = 1000.f;

	// Насколько близко должен стоять САМ ПЕРСОНАЖ, чтобы объект засчитался.
	// Считается от персонажа, а не от камеры — поэтому не зависит от того,
	// как далеко отодвинут CameraBoom.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float MaxInteractDistance = 300.f;

	// Радиус сферы трейса. 0 — тонкий луч (придётся целиться точно),
	// 10-20 — заметно дружелюбнее к игроку.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	float TraceRadius = 15.f;

	// Рисовать луч в игре — удобно при настройке дистанций.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Trace")
	bool bDrawDebugTrace = false;

public:
	// --- Делегаты плашки интеракта (источник — трейс от камеры) ---
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionTargetChanged OnTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionTargetCleared OnTargetCleared;

	// --- Делегаты диалогового окна (источник — EventObject) ---
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnWorldObjectMessage OnWorldObjectMessage;

	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnWorldObjectMessageCleared OnWorldObjectMessageCleared;

	// Нужно InteractableBase, чтобы старые оверлапы не спорили с трейсом.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsUsingLineTrace() const { return bUseLineTrace; }

	// Текущая цель (может пригодиться в блюпринтах, например для подсветки).
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }

	// Есть ли сейчас цель. Удобно вешать на Visibility виджета.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool HasInteractionTarget() const { return CurrentInteractable != nullptr; }

	// Куда упёрся трейс. Гнать через Project World Location to Widget Position,
	// чтобы поставить ромб-якорь ровно на предмет. Читать каждый тик, пока
	// виджет открыт: игрок крутит камеру, и экранная точка всё время едет.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FVector GetLastImpactPoint() const { return LastImpactPoint; }

	// Description текущей цели одной нодой — чтобы в виджете не городить
	// Cast To WorldObject каждый раз. Пусто, если цели нет.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FText GetCurrentDescription() const;

	// Всё про текущую цель одной нодой, тот же набор, что приходит в делегат.
	// Нужно, если виджет хочет опрашивать данные сам на Tick, не подписываясь
	// на событие. Если цели нет — структура с пустыми полями и StateIndex = -1.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FInteractionInfo GetCurrentInteractionInfo() const;

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
