


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

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
