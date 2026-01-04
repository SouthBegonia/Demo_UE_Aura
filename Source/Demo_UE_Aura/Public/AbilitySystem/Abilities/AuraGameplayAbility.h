

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	FGameplayTag StartupInputTag;

#pragma region Ability Description

	virtual FString GetAbilityDescription(int32 AbilityLevel);
	virtual FString GetNextAbilityDescription(int32 AbilityLevel);
	static FString GetLockedDescription(int32 Level);

#pragma endregion

protected:

	float GetManaCost(float InAbilityLevel = 1.f) const;
	float GetCooldown(float InAbilityLevel = 1.f) const;
};
