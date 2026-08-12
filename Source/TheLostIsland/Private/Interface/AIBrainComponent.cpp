#include "Interface/AIBrainComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIBrainComponent::UAIBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIBrainComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAIBrainComponent::SetBlackboardInt(FName KeyName, int32 Value)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner) return;

	AAIController* AIComp = Cast<AAIController>(PawnOwner->GetController());
	if (AIComp && AIComp->GetBlackboardComponent())
	{
		AIComp->GetBlackboardComponent()->SetValueAsInt(KeyName, Value);
	}
}

void UAIBrainComponent::SetBlackboardVector(FName KeyName, FVector Value)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner) return;

	AAIController* AIComp = Cast<AAIController>(PawnOwner->GetController());
	if (AIComp && AIComp->GetBlackboardComponent())
	{
		AIComp->GetBlackboardComponent()->SetValueAsVector(KeyName, Value);
	}
}