


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "Kismet/KismetMathLibrary.h"

// One of the ways to Apply Damage GE (ex. GA_MeleeAttack
void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	// set all Types of Damage by CallerMagnitude
	const float ScaleDamage = GetDamageValue(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GetDamageType(), ScaleDamage);


	// Apply GE to Target
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return GetDamageValue(GetAbilityLevel());
}

// One of the ways to Apply Damage GE, then will call UAuraAbilitySystemLibrary::ApplyDamageEffect()  (ex. GA_FireBolt
FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();

	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageEffectClass = GetDamageEffectClass();
	Params.BaseDamage = GetDamageValue(GetAbilityLevel());
	Params.DamageType = GetDamageType();

	Params.DebuffConfig = GetAbilityDebuffConfig();
	Params.DebuffConfig.DebuffType = FAuraGameplayTags::Get().DamageTypesToDebuffTypeMap[GetDamageType()];

	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;

	if (IsValid(TargetActor))
	{
		const FRotator FaceTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetAvatarActorFromActorInfo()->GetActorLocation(), TargetActor->GetActorLocation());

		const FVector DeathImpulse = FaceTargetRotation.Vector();
		Params.DeathImpulse = DeathImpulse;

		if (const bool bKnockback = FMath::RandRange(1, 100) < Params.KnockbackChance)
		{
			FRotator KnockbackRotation = FaceTargetRotation;
			KnockbackRotation.Pitch = 45.f;
			const FVector KnockbackDirection = KnockbackRotation.Vector();
			Params.KnockbackForce = KnockbackDirection * Params.KnockbackForceMagnitude;
		}
	}

	return Params;
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
