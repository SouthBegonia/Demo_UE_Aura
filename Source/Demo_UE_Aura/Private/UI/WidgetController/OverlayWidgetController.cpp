


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	const float Health = AuraAttributeSet->GetHealth();
	const float MaxHealth = AuraAttributeSet->GetMaxHealth();
	OnHealthChanged.Broadcast(Health);
	OnMaxHealthChanged.Broadcast(MaxHealth);
	const float Mana = AuraAttributeSet->GetMana();
	const float MaxMana = AuraAttributeSet->GetMaxMana();
	OnManaChanged.Broadcast(Mana);
	OnMaxManaChanged.Broadcast(MaxMana);
}

void UOverlayWidgetController::BindCallbackToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& AttributeChangeData)
	{
		OnHealthChanged.Broadcast(AttributeChangeData.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& AttributeChangeData)
	{
		OnMaxHealthChanged.Broadcast(AttributeChangeData.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& AttributeChangeData)
	{
		OnManaChanged.Broadcast(AttributeChangeData.NewValue);
	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& AttributeChangeData)
	{
		OnMaxManaChanged.Broadcast(AttributeChangeData.NewValue);
	});

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
	{
		for (const FGameplayTag Tag : AssetTags)
		{
			//const FString Msg = FString::Printf(TEXT("GE Tag = %s"), *Tag.ToString());
			//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);

			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message"))))
			{
				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	});
}
