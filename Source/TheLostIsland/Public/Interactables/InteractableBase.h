// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "GameplayTagContainer.h"
#include "InteractableBase.generated.h"

class USoundBase;
//class UNiagaraSystem;
class UAnimMontage;
class AGameManager;

USTRUCT(BlueprintType)
struct FInteractionState
{
	GENERATED_BODY()

	// Description shown in interaction window.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Description;

	// Action shown to the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionText;
};

UCLASS()
class THELOSTISLAND_API AInteractableBase : public AActor, public IInteractableInterface
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
	//Display name show in interaction widget.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText DisplayName;
	// All interaction states for this object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<FInteractionState> States;
	// Current state index.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	int32 CurrentState = 0;
	// Can this object currently be interacted with?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bCanInteract = true;
	// True until the first successful interaction.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bFirstInteraction = true;
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

	///State///
	virtual void OnStateChanged();

public:	

	//Interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact();
	virtual void Interact_Implementation();
	virtual FText GetDisplayName() const;
	virtual FText GetDescription() const;
	virtual FText GetActionText() const;
	virtual FGameplayTag GetInteractionEvent() const override;
	// Sets the current interaction state.
	UFUNCTION(BlueprintCallable)
	void SetCurrentState(int32 NewState);
	// Returns the current interaction state.
	UFUNCTION(BlueprintPure)
	int32 GetCurrentState() const;


};
