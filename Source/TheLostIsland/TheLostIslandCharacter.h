// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Interface/InteractableInterface.h"
#include "TheLostIslandCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class AGameManager;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ATheLostIslandCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	// Current actor implementing interaction interface.
	UPROPERTY()
	AActor* CurrentInteractable;

	// Reference to the game manager.
	UPROPERTY()
	AGameManager* GameManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float WorldObjectDisplayTime = 1.5f;

public:

	/** Constructor */
	ATheLostIslandCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for looking interact */
	void Interact();

	FTimerHandle WorldObjectWidgetTimerHandle;

	void HideWorldObjectWidget();

	

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* NewInteractable);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentWorldObject(AWorldObject* NewWorldObject);

public:

	void ClearCurrentInteractable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void ShowInteractionWidget(
		const FText& DisplayName,
		const FText& ActionText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void HideInteractionWidget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RefreshInteractionWidget();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

