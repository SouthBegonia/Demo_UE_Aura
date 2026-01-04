


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAttributeSet.h"

#pragma region Ability Description

FString UAuraGameplayAbility::GetAbilityDescription(const int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s : </><Level>%d</>"), TEXT("Default Ability Name"), AbilityLevel);
}

FString UAuraGameplayAbility::GetNextAbilityDescription(const int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>Next Level : </><Level>%d</>\n<Default>Causes much more damage </>"), AbilityLevel);
}

FString UAuraGameplayAbility::GetLockedDescription(const int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level : %d</>"), Level);
}

#pragma endregion


float UAuraGameplayAbility::GetManaCost(float InAbilityLevel) const
{
	float ManaCost = 0.0f;

	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& ModifierInfo : CostGE->Modifiers)
		{
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{

				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(InAbilityLevel, ManaCost);
				return FMath::Abs(ManaCost);
			}
		}
	}

	UE_LOGFMT(LogAura, Warning, "[{FUNC}] : ManaCost was not obtained with the expected process", __FUNCTION__);
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InAbilityLevel) const
{
	float Cooldown = 0.0f;
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		if (CooldownGE->DurationPolicy == EGameplayEffectDurationType::HasDuration)
		{
			CooldownGE->DurationMagnitude.GetStaticMagnitudeIfPossible(InAbilityLevel, Cooldown);
			return Cooldown;
		}
	}

	UE_LOGFMT(LogAura, Warning, "[{FUNC}] : Cooldown was not obtained with the expected process", __FUNCTION__);
	return Cooldown;
}
