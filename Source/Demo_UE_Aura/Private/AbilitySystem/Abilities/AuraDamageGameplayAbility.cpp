


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraLogChannels.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	// set all Types of Damage by CallerMagnitude
	for (const TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypesMap)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
	}

	// Apply GE to Target
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 SelectionIndex = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[SelectionIndex];
	}

	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InAbilityLevel, const FGameplayTag& DamageTypeTag) const
{
	check(DamageTypesMap.Contains(DamageTypeTag))
	return DamageTypesMap[DamageTypeTag].GetValueAtLevel(InAbilityLevel);
}
