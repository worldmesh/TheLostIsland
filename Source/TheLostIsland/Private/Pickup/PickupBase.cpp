// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup/PickupBase.h"
#include "Sound/SoundBase.h"
//#include "NiagaraSystem.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TheLostIslandCharacter.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(Mesh);

	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnInteractionBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &APickupBase::OnInteractionBoxEndOverlap);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupBase::OnInteractionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATheLostIslandCharacter* Character = Cast<ATheLostIslandCharacter>(OtherActor);

	if (Character)
	{
		Character->SetCurrentInteractable(this);
	}
}

void APickupBase::OnInteractionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATheLostIslandCharacter* Character = Cast<ATheLostIslandCharacter>(OtherActor);

	if (Character)
	{
		Character->ClearCurrentInteractable();
	}
}

void APickupBase::Interact()
{
	AGameManager* GameManager =
		Cast<AGameManager>(
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				AGameManager::StaticClass()));

	if (GameManager)
	{
		GameManager->FuelPickedUp();
	}

	Destroy();
}

FText APickupBase::GetDisplayName_Implementation() const
{
	return DisplayName;
}

FText APickupBase::GetActionText_Implementation() const
{
	return ActionText;
}

