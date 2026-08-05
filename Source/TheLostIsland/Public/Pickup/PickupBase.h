// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupBase.generated.h"


class USoundBase;
//class UNiagaraSystem;
class UAnimMontage;

UCLASS()
class THELOSTISLAND_API APickupBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionBox;

	//Display name show in interaction widget.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText ActionText = FText::FromString(TEXT("[E] Подобрать"));

	// Interaction feedback.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	class USoundBase* InteractSound;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	//class UNiagaraSystem* InteractEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	class UAnimMontage* InteractMontage;

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
	virtual void Interact() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetDisplayName() const;
	virtual FText GetDisplayName_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetActionText() const;
	virtual FText GetActionText_Implementation() const;

};
