


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbackToDependencies()
{
	UAuraAttributeSet* AS = GetAuraAS();
	AAuraPlayerState* AuraPlayerState = GetAuraPS();

	check(AttributeInfo)

	// GameplayAttributeValue Events
	for (auto& Pair : AS->TagsToAttributes)
	{
		FGameplayAttribute Attr = Pair.Value();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attr).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}

	// AttributePoints, SpellPoints Events
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda([&](const int32 NewPoints)
	{
		AttributePointsChangedDelegate.Broadcast(NewPoints);
	});
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([&](const int32 NewPoints)
	{
		SpellPointsChangedDelegate.Broadcast(NewPoints);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = GetAuraAS();
	AAuraPlayerState* AuraPlayerState = GetAuraPS();

	check(AttributeInfo)

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);

	AttributeInfoDelegate.Broadcast(Info);
}

