// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "WorldObject/WorldObject.h"
#include "GameplayTagContainer.h"
#include "InteractableBase.generated.h"

class USoundBase;
//class UNiagaraSystem;
class UAnimMontage;
class AGameManager;


UCLASS()
class THELOSTISLAND_API AInteractableBase : public AWorldObject, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	///Components///
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionBox;


	///Interaction Data///
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FGameplayTag InteractionEvent;
	
	// Interaction feedback.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	class USoundBase* InteractSound;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	//class UNiagaraSystem* InteractEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	class UAnimMontage* InteractMontage;
	
	///Collision Overlaps///
	UFUNCTION()
	void OnInteractionBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);


public:	

	//Interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact();
	virtual void Interact_Implementation();
	virtual FText GetDisplayName() const override;
	virtual FText GetDescription() const;
	virtual FText GetActionText() const;
	virtual FGameplayTag GetInteractionEvent() const override;



};
