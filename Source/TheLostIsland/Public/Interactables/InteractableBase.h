// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

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
class THELOSTISLAND_API AInteractableBase : public AActor
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

	//Called when player interact with this object.
	virtual void Interact();

	// Returns interaction display name.
	virtual FText GetDisplayName() const;

	// Returns interaction description.
	virtual FText GetDescription() const;

	// Returns interaction action text.
	virtual FText GetActionText() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
