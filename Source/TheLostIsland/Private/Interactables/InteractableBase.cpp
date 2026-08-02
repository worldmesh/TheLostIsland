// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TheLostIslandCharacter.h"

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

void AInteractableBase::OnInteractionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.0f, FColor::Green, FText::Format(FText::FromString(TEXT("Вы пересекли {0}")), DisplayName).ToString()
		);
	}
	
	ATheLostIslandCharacter* Character = Cast<ATheLostIslandCharacter>(OtherActor);

	if (Character)
	{
		Character->SetCurrentInteractable(this);
	}
}

void AInteractableBase::OnInteractionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATheLostIslandCharacter* Character = Cast<ATheLostIslandCharacter>(OtherActor);

	if (Character)
	{
		Character->ClearCurrentInteractable();
	}
}

void AInteractableBase::Interact()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Cyan,
			FString::Printf(TEXT("Взаимодействие: %s"), *DisplayName.ToString())
		);
	}
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FText AInteractableBase::GetDisplayName() const
{
	return DisplayName;
}

FText AInteractableBase::GetDescription() const
{
	return Description;
}

FText AInteractableBase::GetActionText() const
{
	return ActionText;
}