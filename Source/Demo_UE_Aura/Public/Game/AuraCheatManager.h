

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "AuraCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void AuraEnemyFreeze() const;
	UFUNCTION(Exec)
	void AuraEnemyFreezeRecover();
};
