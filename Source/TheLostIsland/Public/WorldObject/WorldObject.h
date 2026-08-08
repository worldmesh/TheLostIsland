// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObject.generated.h"



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
class THELOSTISLAND_API AWorldObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldObject();

protected:
	
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Object")
	FText DisplayName;
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Object")
	TArray<FInteractionState> States;
	// Current state index.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Object")
	int32 CurrentState = 0;
	virtual void OnStateChanged();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Object")
	bool bCanInteract = true;
	// True until the first successful interaction.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Object")
	bool bFirstInteraction = true;
	void NotifyGameManager();


public:	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Object")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Object")
	FText GetDescription() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "World Object")
	FText GetActionText() const;
	UFUNCTION(BlueprintCallable)
	bool SetCurrentState(int32 NewState);
	// Returns the current interaction state.
	UFUNCTION(BlueprintPure)
	int32 GetCurrentState() const;
};
