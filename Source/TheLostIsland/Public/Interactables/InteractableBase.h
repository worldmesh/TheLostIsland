// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "WorldObject/WorldObject.h"
#include "GameplayTagContainer.h"
#include "InteractableBase.generated.h"

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
	// Точка крепления виджета подсказки интеракта. Двигается вручную в каждом
	// Blueprint-наследнике (на борт лодки, на дверь маяка, на гнездо предохранителя).
	// По умолчанию совпадает с Mesh (Actor Location) — двигать не обязательно.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* WidgetAnchor;


	///Interaction Data///

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FGameplayTag InteractionEvent;

	// Материал подсветки «на меня смотрят». Ставится поверх меша отдельным
	// проходом (Overlay Material) в SetInteractHighlighted. Выставляется один раз
	// в Class Defaults у BP_BaseInteract — наследуется всеми объектами.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	class UMaterialInterface* HighlightMaterial;
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

	// Мировая позиция для виджета подсказки интеракта. Читает WidgetAnchor;
	// если он почему-то не создан — фолбэк на GetActorLocation().
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FVector GetWidgetAnchorLocation() const;

	// Подсветка меша под прицелом (Custom Depth). Mesh — protected, поэтому
	// снаружи (например из WBP_InteractPrompt) включать/выключать подсветку
	// нужно через эту функцию, а не напрямую.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractHighlighted(bool bNewHighlighted);



};
