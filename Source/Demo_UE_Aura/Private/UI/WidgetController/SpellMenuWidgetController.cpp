


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbackToDependencies()
{
	AAuraPlayerState* AuraPlayerState = GetAuraPS();

	// Bind Event - AbilityInfo
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if 	(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;

			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	// Bind Event - SpellPointsChanged
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([&](const int32 CurrentPoints)
	{
		SpellPointsChangedDelegate.Broadcast(CurrentPoints);
	});
}
