


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* OwningPawn = AIOwner->GetPawn();
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, FoundActors);
	float CloestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* const& FoundActor : FoundActors)
	{
		if (IsValid(FoundActor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(FoundActor);
			if (Distance < CloestDistance)
			{
				CloestDistance = Distance;
				ClosestActor = FoundActor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, CloestDistance);
}
