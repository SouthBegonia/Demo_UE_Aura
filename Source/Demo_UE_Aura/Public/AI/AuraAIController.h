

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAuraAIController();

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;
};
