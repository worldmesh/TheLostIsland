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

}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupBase::Interact_Implementation()
{
    if (SetCurrentState(1))
    {
        NotifyGameManager();
    }

	Destroy();
}



