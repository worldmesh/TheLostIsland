#include "WorldObject/EventObject.h"
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

void AEventObject::OnEventBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (OtherActor->IsA<ATheLostIslandCharacter>())
	{
		UE_LOG(LogTemp, Warning, TEXT("EventObject: Player entered event zone"));
	}
}