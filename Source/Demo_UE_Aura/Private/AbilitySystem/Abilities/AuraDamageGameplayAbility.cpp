


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraLogChannels.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	// set all Types of Damage by CallerMagnitude
	const float ScaleDamage = GetDamageValue(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, ScaleDamage);


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

float UAuraDamageGameplayAbility::GetDamageValue(float InAbilityLevel) const
{
	return DamageScalableFloat.GetValueAtLevel(InAbilityLevel);
}
