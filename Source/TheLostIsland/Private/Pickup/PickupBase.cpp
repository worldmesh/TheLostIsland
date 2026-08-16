// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/PickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Interface/InteractionComponent.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupBase::Interact_Implementation()
{
    if (!bCanInteract)
    {
        // Защита от повторного нажатия E, пока идёт таймер до уничтожения
        return;
    }

    // SetCurrentState сам вызывает OnStateChanged() -> NotifyGameManager(),
    // поэтому отдельный вызов здесь не нужен: он гонял EvaluateTransitions дважды.
    SetCurrentState(1);

    // Блокируем повторный интеракт
    bCanInteract = false;

    // Прячем меш сразу — визуально предмет "подобран"
    if (Mesh)
    {
        Mesh->SetVisibility(false, true);
    }

    // Сам актор и коллизия ещё живы DisplayTime секунд, чтобы виджет
    // успел показать текст нового состояния (например "Канистра подобрана")
    GetWorldTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &APickupBase::DestroyPickup,
        GetDisplayTime(),
        false
    );
}

void APickupBase::DestroyPickup()
{
    // Явно прячем виджет у всех, кто сейчас в зоне интеракции —
    // не полагаемся на то, что Destroy() сам корректно пришлёт EndOverlap
    if (InteractionBox)
    {
        TArray<AActor*> OverlappingActors;
        InteractionBox->GetOverlappingActors(OverlappingActors);

        for (AActor* OverlappingActor : OverlappingActors)
        {
            if (UInteractionComponent* InteractionComp = OverlappingActor->FindComponentByClass<UInteractionComponent>())
            {
                InteractionComp->ClearCurrentInteractable();
            }
        }
    }

    Destroy();
}


