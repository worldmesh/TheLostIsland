#pragma once

#include "CoreMinimal.h"
#include "WorldObject.h"
#include "Components/BoxComponent.h"
#include "EventObject.generated.h"

UCLASS()
class THELOSTISLAND_API AEventObject : public AWorldObject
{
	GENERATED_BODY()

public:
	AEventObject();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Event")
	UBoxComponent* EventTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	AWorldObject* TargetObject = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	int32 RequiredState = 0;

	UFUNCTION()
	void OnEventBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};