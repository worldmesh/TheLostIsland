// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/InteractionComponent.h"
#include "Interface/InteractableInterface.h"
#include "WorldObject/WorldObject.h" // Убедитесь, что путь к файлу верный для вашего проекта
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Теперь компонент каждый тик стреляет лучом из камеры, чтобы понять,
	// на что смотрит игрок — поэтому Tick включён.
	PrimaryComponentTick.bCanEverTick = true;

	CurrentInteractable = nullptr;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bUseLineTrace)
	{
		UpdateInteractionTrace();
	}
}

void UInteractionComponent::UpdateInteractionTrace()
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();

	if (!World || !Owner)
	{
		return;
	}

	// 1. Откуда смотрит игрок. GetPlayerViewPoint отдаёт реальную позицию
	//    камеры — в третьем лице это точка позади персонажа.
	APawn* OwnerPawn = Cast<APawn>(Owner);
	APlayerController* PlayerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (!PlayerController)
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * TraceDistance;

	// 2. Сам трейс. Сферой, а не тонким лучом — так игроку не нужно
	//    целиться пиксель в пиксель.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.bTraceComplex = false;

	FHitResult Hit;

	const bool bHitSomething = World->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams);

	// Запоминаем точку попадания ДО всех проверок на дистанцию и интерфейс:
	// виджету она нужна каждый кадр, а не только когда цель сменилась.
	if (bHitSomething)
	{
		LastImpactPoint = Hit.ImpactPoint;
	}
	else
	{
		LastImpactPoint = TraceEnd;
	}

	if (bDrawDebugTrace)
	{
		DrawDebugLine(World, TraceStart, TraceEnd,
			bHitSomething ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);

		if (bHitSomething)
		{
			DrawDebugSphere(World, Hit.ImpactPoint, TraceRadius, 12, FColor::Green, false, 0.f);
		}
	}

	// 3. Решаем, годится ли то, во что попали, как цель взаимодействия.
	AActor* NewTarget = nullptr;

	if (bHitSomething)
	{
		AActor* HitActor = Hit.GetActor();

		// Объект должен уметь взаимодействовать...
		if (HitActor && Cast<IInteractableInterface>(HitActor))
		{
			// ...и персонаж должен стоять достаточно близко.
			// Меряем до ТОЧКИ ПОПАДАНИЯ, а не до центра актора: у крупных
			// объектов (дом, лодка, маяк) центр может быть очень далеко,
			// и по расстоянию до центра интеракт бы не срабатывал.
			const float DistanceToOwner = FVector::Dist(
				Owner->GetActorLocation(),
				Hit.ImpactPoint);

			if (DistanceToOwner <= MaxInteractDistance)
			{
				NewTarget = HitActor;
			}
		}
	}

	// 4. Дёргаем делегаты ТОЛЬКО если что-то реально изменилось,
	//    иначе виджет будет перерисовываться каждый кадр.
	if (NewTarget)
	{
		if (NewTarget != CurrentInteractable)
		{
			SetCurrentInteractable(NewTarget);
		}
	}
	else if (CurrentInteractable || bTargetWidgetShown)
	{
		// Второе условие обязательно: если объект уничтожил сам себя (пикап),
		// CurrentInteractable уже обнулён движком, и без флага виджет остался бы
		// висеть на экране навсегда.
		ClearCurrentInteractable();
	}
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
		bTargetWidgetShown = true;
		OnTargetChanged.Broadcast(MakeInteractionInfo(CurrentInteractable));
	}
}

FText UInteractionComponent::GetCurrentDescription() const
{
	if (const AWorldObject* WorldObject = Cast<AWorldObject>(CurrentInteractable))
	{
		return WorldObject->GetDescription();
	}

	return FText::GetEmpty();
}

FInteractionInfo UInteractionComponent::MakeInteractionInfo(AActor* Target) const
{
	FInteractionInfo Info;
	Info.Actor = Target;

	if (!Target)
	{
		return Info;
	}

	// AWorldObject знает про себя всё — берём данные прямо у него.
	// Сюда попадают и обычные интерактивные объекты, и EventObject.
	if (const AWorldObject* WorldObject = Cast<AWorldObject>(Target))
	{
		Info.DisplayName = WorldObject->GetDisplayName();
		Info.ActionText  = WorldObject->GetActionText();
		Info.Description = WorldObject->GetDescription();
		Info.StateIndex  = WorldObject->GetCurrentState();
		Info.StateName   = WorldObject->GetStateName(Info.StateIndex);
		Info.DisplayTime = WorldObject->GetDisplayTime();

		return Info;
	}

	// Не WorldObject, но умеет взаимодействовать — берём то, что даёт интерфейс.
	// Description/StateName останутся пустыми: у интерфейса их просто нет.
	if (const IInteractableInterface* Interactable = Cast<IInteractableInterface>(Target))
	{
		Info.DisplayName = Interactable->GetDisplayName();
		Info.ActionText  = Interactable->GetActionText();
	}

	return Info;
}

FInteractionInfo UInteractionComponent::GetCurrentInteractionInfo() const
{
	return MakeInteractionInfo(CurrentInteractable);
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
		bTargetWidgetShown = true;
		OnTargetChanged.Broadcast(MakeInteractionInfo(CurrentInteractable));
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

	const float DisplayTime = NewWorldObject->GetDisplayTime();

	// Показываем текст World Object через СВОЙ делегат, не через делегат трейса.
	// bTargetWidgetShown тут НЕ трогаем: этот виджет ставит не трейс, а событие,
	// и прячет его таймер на DisplayTime.
	// Info.DisplayTime внутри — то же самое число, что уйдёт в таймер ниже:
	// если в UMG будет анимация затухания, ей есть на что опереться.
	OnWorldObjectMessage.Broadcast(MakeInteractionInfo(NewWorldObject));

	// Запускаем таймер скрытия
	World->GetTimerManager().SetTimer(
		WorldObjectWidgetTimerHandle,
		this,
		&UInteractionComponent::HideWorldObjectWidget,
		DisplayTime,
		false
	);
}

void UInteractionComponent::HideInteractionWidget()
{
	// Гасим ПЛАШКУ ИНТЕРАКТА. Вызывается из ClearCurrentInteractable(),
	// то есть когда игрок отвёл взгляд или цель исчезла.
	bTargetWidgetShown = false;
	OnTargetCleared.Broadcast();
}

void UInteractionComponent::HideWorldObjectWidget()
{
	// Гасим ДИАЛОГОВОЕ ОКНО. Вызывается только таймером из SetCurrentWorldObject().
	// Плашку интеракта не трогает — это принципиально: раньше отведённый
	// взгляд обрывал диалог на полуслове.
	OnWorldObjectMessageCleared.Broadcast();
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

		// 1.5 Если объект в результате интеракта уже уничтожил себя —
		// не пытаемся читать у него текст, просто прячем виджет
		if (!IsValid(CurrentInteractable))
		{
			ClearCurrentInteractable();
			return;
		}

		// 2. СРАЗУ же обновляем виджет, чтобы отобразить новое состояние и текст!
		RefreshInteractionWidget();
	}
}
