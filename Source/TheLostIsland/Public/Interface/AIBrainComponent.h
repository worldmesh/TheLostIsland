#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIBrainComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THELOSTISLAND_API UAIBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIBrainComponent();

	// Behavior Tree ассет, который назначается персонажу/юниту
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// Вспомогательная функция для обновления интового ключа в Blackboard (например, LighthouseState)
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetBlackboardInt(FName KeyName, int32 Value);

	// Вспомогательная функция для обновления вектора в Blackboard (например, TargetLocation)
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetBlackboardVector(FName KeyName, FVector Value);

protected:
	virtual void BeginPlay() override;
};