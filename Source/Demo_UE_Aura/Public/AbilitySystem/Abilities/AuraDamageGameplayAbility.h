

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	/*
	 * Apply Damage GE to Target
	 */
	UFUNCTION(BlueprintCallable, Category = "Aura|Damage", meta=(ToolTip="Apply Damage GE to Target"))
	void CauseDamage(AActor* TargetActor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/*
	 * Which type of Damage does this GA can execute
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypesMap;


	UFUNCTION(BlueprintPure, Category="Aura")
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
