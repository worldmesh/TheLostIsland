// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameManager.generated.h"

class AWorldObject;

USTRUCT(BlueprintType)
struct FCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	AWorldObject* Object = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	int32 RequiredState = 0;
};

USTRUCT(BlueprintType)
struct FAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	AWorldObject* Object = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	int32 NewState = 0;
};

USTRUCT(BlueprintType)
struct FTransition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC")
	FText TransitionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC")
	TArray<FCondition> Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "01 GAME LOGIC")
	TArray<FAction> Actions;
};

UCLASS()
class THELOSTISLAND_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameManager();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsEvaluatingTransitions = false;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
	TArray<FTransition> Transitions;

	UFUNCTION(BlueprintCallable)
	void EvaluateTransitions();

		

};
