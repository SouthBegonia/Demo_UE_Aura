

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "ArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumShards = 11;

#pragma region Ability Description

public:
	virtual FString GetAbilityDescription(int32 AbilityLevel) override;
	virtual FString GetNextAbilityDescription(int32 AbilityLevel) override;

#pragma endregion
};
