// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/InteractableBase.h"
#include "PickupBase.generated.h"


class USoundBase;
//class UNiagaraSystem;
class UAnimMontage;

UCLASS()
class THELOSTISLAND_API APickupBase : public AInteractableBase {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	//Interaction
	virtual void Interact_Implementation() override;
};
