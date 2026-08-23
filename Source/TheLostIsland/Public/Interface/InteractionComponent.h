// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AActor;
class AWorldObject;

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
	float DisplayTime = 1.5f;

	// Пауза перед показом текста, в секундах — берётся из текущего стейта
	// (FInteractionState::MessageDelay). Нужна диалоговому окну: сообщение
	// должно появляться ПОСЛЕ звука/секвенсера, а не одновременно с ними.
	// Ноль = показывать сразу.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	float MessageDelay = 1.5f;

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

// 3) OnInteractionMessage — ТОЛЬКО результат нажатия [E]. Третий канал.
//
// Диалоговое окно раньше слушало AGameManager::OnObjectStateChanged и потому
// озвучивало ЛЮБУЮ смену состояния, включая каскадные: подобрал предохранитель —
// окно рассказывает про генератор на другом конце карты, запустил генератор —
// про рычаг на маяке. Игрок в этот момент занят другим, текст пролетает мимо.
//
// Здесь гарантированно говорит тот объект, на который игрок нажал, и только он.
// Каскад состояний при этом никуда не девается — GameManager по-прежнему
// переключает меши, свет и звук, он просто перестал быть источником СООБЩЕНИЙ.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionMessage, const FInteractionInfo&, Info);

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
	// Живая, меняется каждый кадр. Нужна для служебных вещей (отладка,
	// эффекты). Виджету для якоря брать НЕ её, а TargetAnchorPoint.
	FVector LastImpactPoint = FVector::ZeroVector;

	// Замороженная точка привязки виджета. Ставится один раз — в момент,
	// когда цель захвачена, — и дальше не двигается, пока цель та же.
	// Смысл: пока игрок читает плашку, она должна стоять на месте.
	// Живой ImpactPoint для этого не годится — от него ромб с выноской
	// ползёт по поверхности предмета за каждым движением мыши, и текст
	// вместе с ним. Глаз не успевает зацепиться.
	FVector TargetAnchorPoint = FVector::ZeroVector;

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

	// Проверка удержания «липкой» цели: луч уже мимо, но не пора ли ещё
	// отпускать? true — держим текущую цель, false — отпускаем.
	// Вынесено отдельной функцией, чтобы UpdateInteractionTrace() читался.
	bool ShouldKeepStickyTarget(const FVector& ViewLocation, const FVector& ViewDirection) const;

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
	float TraceRadius = 25.f;

	// --- «Липкая» цель (гистерезис) ---
	//
	// Захват и отпускание — разные пороги. Захватываем строго: луч должен
	// попасть в объект. А отпускаем мягко: пока игрок не отвёл взгляд
	// заметно, не отошёл и не навёлся на другой интерактив — цель держится,
	// даже если луч уже соскользнул мимо. Без этого плашка мигает от
	// дрожания мыши на краю предмета.

	// Выключатель на случай, если захочется сравнить со старым поведением.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Sticky")
	bool bStickyTarget = true;

	// Насколько далеко от центра экрана можно увести взгляд, не потеряв цель.
	// Считается как угол между направлением камеры и направлением на точку
	// захвата. 20-30 — комфортно; больше 45 — цель начинает «липнуть» слишком
	// сильно и мешает переключаться.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Sticky",
		meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float StickyReleaseAngle = 25.f;

	// Запас по дистанции на отпускание, в долях от MaxInteractDistance.
	// 1.15 = отойти можно на 15% дальше, чем нужно было для захвата.
	// Иначе на самой границе радиуса плашка мигает от шага туда-сюда.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Sticky",
		meta = (ClampMin = "1.0", ClampMax = "2.0"))
	float StickyDistanceTolerance = 1.15f;

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

	// --- Делегат сообщения по результату интеракта ---
	// На него подписано диалоговое окно. Пустой Description фильтруется
	// в виджете, а не здесь: решение «показывать или молчать» принимает тот,
	// кто показывает.
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionMessage OnInteractionMessage;

	// Нужно InteractableBase, чтобы старые оверлапы не спорили с трейсом.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsUsingLineTrace() const { return bUseLineTrace; }

	// Текущая цель (может пригодиться в блюпринтах, например для подсветки).
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }

	// Есть ли сейчас цель. Удобно вешать на Visibility виджета.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool HasInteractionTarget() const { return CurrentInteractable != nullptr; }

	// Куда упёрся трейс ПРЯМО СЕЙЧАС. Живая точка, едет каждый кадр.
	// Для якоря виджета не использовать — см. GetTargetAnchorPoint().
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FVector GetLastImpactPoint() const { return LastImpactPoint; }

	// ЭТО брать для ромба-якоря и выноски. Точка в мире, замороженная в момент
	// захвата цели: гнать через Project World Location to Widget Position
	// каждый тик (экранная проекция всё равно меняется, когда игрок ходит
	// и крутит камеру), но сама точка на предмете стоит намертво — плашка
	// не ползает, пока игрок её читает.
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FVector GetTargetAnchorPoint() const { return TargetAnchorPoint; }

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
