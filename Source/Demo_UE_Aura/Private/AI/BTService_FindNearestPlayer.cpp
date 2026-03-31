


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

	FindAndSetNearestPlayer();
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FindAndSetNearestPlayer();
}

void UBTService_FindNearestPlayer::FindAndSetNearestPlayer()
{
	const APawn* OwningPawn = AIOwner->GetPawn();
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, FoundActors);
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* const& FoundActor : FoundActors)
	{
		if (IsValid(FoundActor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(FoundActor);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = FoundActor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
