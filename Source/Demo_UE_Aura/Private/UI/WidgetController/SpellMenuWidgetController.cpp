


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
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
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 AbilityLevel)
	{
		if 	(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;

			AbilityInfoDelegate.Broadcast(Info);
		}

		// Update SelectedSpellGlobe when AbilityStatus changed
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.AbilityStatusTag = StatusTag;

			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
			BroadcastSpellDescriptionUpdate(SelectedAbility.AbilityTag);
		}
	});

	// Bind Event - SpellPointsChanged
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([&](const int32 CurrentPoints)
	{
		SpellPointsChangedDelegate.Broadcast(CurrentPoints);

		CurrentSpellPoints = CurrentPoints;

		// Update SelectedSpellGlobe when SpellPoints changed
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.AbilityStatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
		BroadcastSpellDescriptionUpdate(SelectedAbility.AbilityTag);
	});
}

void USpellMenuWidgetController::SpendSpellPointsButtonClicked()
{
	if (GetAuraASC())
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);


	FGameplayTag AbilityStatus = GameplayTags.Abilities_Status_Locked;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);	// unset or deselect
	const bool bSpecValid = AbilitySpec != nullptr;


	if (!bTagValid || bTagNone || !bSpecValid)
	{
		// SpellGlobe Locked
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}

	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

	// Cache data
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = AbilityStatus;

	// Broadcast
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
	BroadcastSpellDescriptionUpdate(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, const int32 SpellPoints, bool& bShouldEnabledSpellPointsButton, bool& bShouldEnabledEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	bShouldEnabledSpellPointsButton = false;
	bShouldEnabledEquipButton = false;

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnabledSpellPointsButton = SpellPoints > 0;
		bShouldEnabledEquipButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnabledSpellPointsButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnabledSpellPointsButton = SpellPoints > 0;
		bShouldEnabledEquipButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
	}
}

void USpellMenuWidgetController::BroadcastSpellDescriptionUpdate(const FGameplayTag& AbilityTag)
{
	if (GetAuraASC())
	{
		FString SpellNextLevelDescription;
		FString SpellDescription;

		// Description will be empty when AbilityTag invalid(unselect a spell) or AbilityTag=Abilities_None
		if (AbilityTag.IsValid() && !AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
			GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, SpellDescription, SpellNextLevelDescription);

		SpellDescriptionChangeDelegate.Broadcast(SpellDescription, SpellNextLevelDescription);
	}
}
