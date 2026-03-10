


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
FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor, FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection,
	FVector KnockbackDirectionOverride, bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
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
	const bool bKnockbackSuccess = FMath::RandRange(1, 100) < Params.KnockbackChance;

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		const FVector ToTarget = Rotation.Vector();

		if (!bOverrideKnockbackDirection && bKnockbackSuccess)
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;

		if (!bOverrideDeathImpulse)
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;

	}

	// Knockback Direction
	if (bOverrideKnockbackDirection && bKnockbackSuccess)
	{
		KnockbackDirectionOverride.Normalize();
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;

		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;

			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}

	// DeathImpulse Direction
	if (bOverrideDeathImpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;

		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;

			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
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
