

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

#pragma region Ability Description

public:
	virtual FString GetAbilityDescription(int32 AbilityLevel) override;
	virtual FString GetNextAbilityDescription(int32 AbilityLevel) override;

#pragma endregion
};
