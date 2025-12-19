


#include "AbilitySystem/Data/AbilityInfo.h"

#include "AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	const FAuraAbilityInfo* AbilityInfoFound = AbilityInformation.FindByPredicate([&AbilityTag](const FAuraAbilityInfo& AbilityInfo)
	{
		return AbilityInfo.AbilityTag == AbilityTag;
	});

	if (AbilityInfoFound == nullptr)
	{
		if (bLogNotFound)
			UE_LOG(LogAura, Error, TEXT("UAbilityInfo::FindAbilityInfoForTag    Can't find AbilityInfo for Tag=[%s] on [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));

		return FAuraAbilityInfo();
	}

	return *AbilityInfoFound;
}
