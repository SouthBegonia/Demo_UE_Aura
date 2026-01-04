

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, const FGameplayTag&, AbilityTypeTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellDescriptionChangeSignature, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityTypeTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag AbilityStatusTag = FGameplayTag();
	FGameplayTag AbilityTypeTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DEMO_UE_AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void OnWidgetDestruct_Implementation() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|GAS|Attributes")
	FOnPlayerStatChangeSignature SpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura")
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura")
	FSpellDescriptionChangeSignature SpellDescriptionChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura")
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbackToDependencies() override;


	UFUNCTION(BlueprintCallable)
	void SpendSpellPointsButtonClicked();

	UFUNCTION(BlueprintCallable)
	void EquipButtonClicked();


	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

private:
	void ShouldEnableButtons(const FGameplayTag& AbilityStatus, const int32 SpellPoints, bool& bShouldEnabledSpellPointsButton, bool& bShouldEnabledEquipButton);

	bool bWaitingForEquipSelection = false;

	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked };
	int32 CurrentSpellPoints = 0;

	void BroadcastSpellDescriptionUpdate(const FGameplayTag& AbilityTag);
};
