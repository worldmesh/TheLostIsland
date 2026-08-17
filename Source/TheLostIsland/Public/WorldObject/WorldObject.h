// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WorldObject.generated.h"

class UStaticMesh;
class USoundBase;
class UAnimationAsset;
class ULevelSequence;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FStateEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	int32 MaterialSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UAnimationAsset* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	ULevelSequence* Sequencer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* Niagara = nullptr;
};

USTRUCT(BlueprintType)
struct FInteractionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString StateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FStateEffects Effects;
};


UCLASS()
class THELOSTISLAND_API AWorldObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldObject();

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC", meta = (TitleProperty = "StateName"))
	TArray<FInteractionState> States;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	int32 CurrentState = 0;
	virtual void OnStateChanged();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	bool bCanInteract = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "01 GAME LOGIC")
	bool bFirstInteraction = true;

	void NotifyGameManager();


public:	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetDescription() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "01 GAME LOGIC")
	FText GetActionText() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float DisplayTime = 1.5f;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetDisplayTime() const;

	UFUNCTION(BlueprintCallable)
	bool SetCurrentState(int32 NewState);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentState() const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	int32 GetStateCount() const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FString GetStateName(int32 StateIndex) const;

	UFUNCTION(BlueprintPure, Category = "01 GAME LOGIC")
	FText GetActionTextForState(int32 StateIndex) const;

	void ApplyStateEffects();
};
