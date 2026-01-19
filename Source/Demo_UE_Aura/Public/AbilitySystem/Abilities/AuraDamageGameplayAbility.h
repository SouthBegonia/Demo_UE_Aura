

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
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

	UFUNCTION(BlueprintPure, Category = "Aura|Damage")
	float GetDamageAtLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aura|Damage")
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE FGameplayTag GetDamageType() const { return DamageTypeTag; }
	float GetDamageValue(float InAbilityLevel) const;

	FORCEINLINE FAbilityDebuffConfig GetAbilityDebuffConfig() const { return DebuffConfig; }

	UFUNCTION(BlueprintPure, Category="Aura")
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FScalableFloat DamageScalableFloat;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage|DamageEffectParams")
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage|DamageEffectParams")
	float KnockbackForceMagnitude = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage|DamageEffectParams")
	float KnockbackChance = 0.f;


#pragma region Debuff

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage|DamageEffectParams", meta = (AllowPrivateAccess = "true"))
	FAbilityDebuffConfig DebuffConfig;

#pragma endregion
};
