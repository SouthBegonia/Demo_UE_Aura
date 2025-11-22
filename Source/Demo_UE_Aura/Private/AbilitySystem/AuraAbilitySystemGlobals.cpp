


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	//return Super::AllocGameplayEffectContext();
	return new FAuraGameplayEffectContext();
}
