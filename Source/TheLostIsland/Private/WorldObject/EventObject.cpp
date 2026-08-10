#include "WorldObject/EventObject.h"
#include "Interface/InteractionComponent.h"
#include "TheLostIslandCharacter.h"

AEventObject::AEventObject()
{
	PrimaryActorTick.bCanEverTick = false;

	EventTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EventTrigger"));
	RootComponent = EventTrigger;

	EventTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EventTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	EventTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	EventTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AEventObject::OnEventBeginOverlap
	);
}

void AEventObject::OnEventBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (UInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UInteractionComponent>())
	{
		// 1. Выбираем сырой AActor (TargetObject или this)
		AActor* RawTarget = TargetObject ? TargetObject : this;

		// 2. Приводим его к AWorldObject* (так как SetCurrentWorldObject ждет именно его)
		if (AWorldObject* WorldObj = Cast<AWorldObject>(RawTarget))
		{
			InteractionComp->SetCurrentWorldObject(WorldObj);
		}
	}
}