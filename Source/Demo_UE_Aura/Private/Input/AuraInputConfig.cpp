


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& AuraInputAction : AbilityInputActions)
	{
		if (AuraInputAction.InputAction && AuraInputAction.InputTag == InputTag)
			return AuraInputAction.InputAction;
	}

	if (bLogNotFound)
		UE_LOG(LogTemp, Error, TEXT("Can't find InputAction for InputTag [%s] on %s"), *InputTag.ToString(), *GetNameSafe(this));

	return nullptr;
}
