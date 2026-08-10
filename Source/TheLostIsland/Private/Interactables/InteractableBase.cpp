// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableBase.h"
#include "Interface/InteractionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TheLostIslandCharacter.h"
#include "Sound/SoundBase.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"


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
			InteractionComp->SetCurrentInteractable(this);
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
			InteractionComp->ClearCurrentInteractable();
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
