

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/*
	 * Which type of Damage does this GA can execute
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypesMap;
};
