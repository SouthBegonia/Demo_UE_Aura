


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::OnWidgetDestruct_Implementation()
{
}

void UAuraWidgetController::BroadcastInitialValues()
{

}

void UAuraWidgetController::BindCallbackToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	// Get AbilityInformation from ASC after StartupAbilitiesGiven, and broadcast it to Widget
	if (!GetAuraASC()->bStartupAbilitiesGiven)
		return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		// execute on each Activatable Ability

		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(GetAuraASC()->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = GetAuraASC()->GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = GetAuraASC()->GetStatusFromSpec(AbilitySpec);

		// Broadcast AbilityInfo to WidgetController
		AbilityInfoDelegate.Broadcast(Info);
	});

	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC(bool bCheckValid)
{
	if (!AuraPlayerControllerPrivate.IsValid())
		AuraPlayerControllerPrivate = Cast<AAuraPlayerController>(PlayerController);

	if (bCheckValid) check(AuraPlayerControllerPrivate.IsValid())

	return AuraPlayerControllerPrivate.Get();
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS(bool bCheckValid)
{
	if (!AuraPlayerStatePrivate.IsValid())
		AuraPlayerStatePrivate = Cast<AAuraPlayerState>(PlayerState);

	if (bCheckValid) check(AuraPlayerStatePrivate.IsValid())

	return AuraPlayerStatePrivate.Get();
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC(bool bCheckValid)
{
	if (!AuraAbilitySystemComponentPrivate.IsValid())
		AuraAbilitySystemComponentPrivate = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	if (bCheckValid) check(AuraAbilitySystemComponentPrivate.IsValid())

	return AuraAbilitySystemComponentPrivate.Get();
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS(bool bCheckValid)
{
	if (!AuraAttributeSetPrivate.IsValid())
		AuraAttributeSetPrivate = Cast<UAuraAttributeSet>(AttributeSet);

	if (bCheckValid) check(AuraAttributeSetPrivate.IsValid())

	return AuraAttributeSetPrivate.Get();
}
