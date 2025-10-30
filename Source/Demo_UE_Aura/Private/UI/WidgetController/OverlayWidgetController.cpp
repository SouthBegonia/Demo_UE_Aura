


#include "UI/WidgetController/OverlayWidgetController.h"

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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& AttributeChangeData) const
{
	const float Health = AttributeChangeData.NewValue;
	OnHealthChanged.Broadcast(Health);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& AttributeChangeData) const
{
	const float MaxHealth = AttributeChangeData.NewValue;
	OnMaxHealthChanged.Broadcast(MaxHealth);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& AttributeChangeData) const
{
	const float Mana = AttributeChangeData.NewValue;
	OnManaChanged.Broadcast(Mana);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& AttributeChangeData) const
{
	const float MaxMana = AttributeChangeData.NewValue;
	OnMaxManaChanged.Broadcast(MaxMana);
}
