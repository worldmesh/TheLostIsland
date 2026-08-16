// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableBase.h"
#include "Interface/InteractionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Mesh);

	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractableBase::OnInteractionBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AInteractableBase::OnInteractionBoxEndOverlap);

}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();

}

void AInteractableBase::OnInteractionBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// Ищем компонент у любого актера, который вошел в триггер
		if (UInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UInteractionComponent>())
		{
			// Если компонент работает по трейсу — цель выбирает он сам,
			// иначе оверлап и трейс будут спорить друг с другом.
			// Бокс при этом остаётся: он по-прежнему нужен PickupBase,
			// чтобы найти всех, кто рядом, перед уничтожением объекта.
			if (!InteractionComp->IsUsingLineTrace())
			{
				InteractionComp->SetCurrentInteractable(this);
			}
		}
	}
}

void AInteractableBase::OnInteractionBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (UInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UInteractionComponent>())
		{
			if (!InteractionComp->IsUsingLineTrace())
			{
				InteractionComp->ClearCurrentInteractable();
			}
		}
	}
}

void AInteractableBase::Interact_Implementation()
{

}


FText AInteractableBase::GetDisplayName() const
{
	return AWorldObject::GetDisplayName();
}

FText AInteractableBase::GetDescription() const
{
	return AWorldObject::GetDescription();
}

FText AInteractableBase::GetActionText() const
{
	return AWorldObject::GetActionText();
}

FGameplayTag AInteractableBase::GetInteractionEvent() const
{
	return InteractionEvent;
}
