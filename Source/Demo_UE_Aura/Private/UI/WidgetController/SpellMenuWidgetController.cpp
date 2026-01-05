


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::OnWidgetDestruct_Implementation()
{
	// Unbind Events when UI close
	SpellPointsChangedDelegate.Clear();
	SpellGlobeSelectedDelegate.Clear();
	SpellDescriptionChangeDelegate.Clear();
	WaitForEquipSelectionDelegate.Clear();

	// Clear Data
	bWaitingForEquipSelection = false;

	Super::OnWidgetDestruct_Implementation();
}

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
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, SelectedAbility.AbilityTag);
			BroadcastSpellDescriptionUpdate(SelectedAbility.AbilityTag);
		}
	});

	// Bind Event - Equipped Ability
	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	// Bind Event - SpellPointsChanged
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([&](const int32 CurrentPoints)
	{
		SpellPointsChangedDelegate.Broadcast(CurrentPoints);

		CurrentSpellPoints = CurrentPoints;

		// Update SelectedSpellGlobe when SpellPoints changed
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.AbilityStatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, SelectedAbility.AbilityTypeTag);
		BroadcastSpellDescriptionUpdate(SelectedAbility.AbilityTag);
	});
}

void USpellMenuWidgetController::SpendSpellPointsButtonClicked()
{
	if (GetAuraASC())
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::EquipButtonClicked()
{
	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	check(AbilityTypeTag.IsValid())

	WaitForEquipSelectionDelegate.Broadcast(AbilityTypeTag);
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobeClicked(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	// SlotTag is equivalent to InputTag

	if (!bWaitingForEquipSelection)
		return;

	check(SlotTag.IsValid() && AbilityType.IsValid())

	// Check selected ability against the slot's ability type (Offensive spell couldn't be equipped Passive slot)
	const FGameplayTag& SelectedAbilityTypeTag = SelectedAbility.AbilityTypeTag;
	if (!SelectedAbilityTypeTag.IsValid() || !SelectedAbilityTypeTag.MatchesTagExact(AbilityType))
		return;

	// Try To Equip Ability
	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, SlotTag);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);


	FGameplayTag AbilityStatus = GameplayTags.Abilities_Status_Locked;
	FGameplayTag AbilityType = GameplayTags.Abilities_Type_None;

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
		AbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityTypeTag;
	}

	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);

	// Cache data
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = AbilityStatus;
	SelectedAbility.AbilityTypeTag = AbilityType;

	// Broadcast
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, SelectedAbility.AbilityTypeTag);
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

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
	bWaitingForEquipSelection = false;

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if Equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	// Broadcast AbilityInfo Equipped
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	WaitForEquipSelectionDelegate.Broadcast(FGameplayTag());
}
