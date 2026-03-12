

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|FireBlast")
	int32 NumFireBalls = 12;

#pragma region Ability Description

public:
	virtual FString GetAbilityDescription(int32 AbilityLevel) override;
	virtual FString GetNextAbilityDescription(int32 AbilityLevel) override;

#pragma endregion
};
