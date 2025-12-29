


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = GetAuraAS();

	const float Health = AuraAttributeSet->GetHealth();
	const float MaxHealth = AuraAttributeSet->GetMaxHealth();
	OnHealthChanged.Broadcast(Health);
	OnMaxHealthChanged.Broadcast(MaxHealth);
	const float Mana = AuraAttributeSet->GetMana();
	const float MaxMana = AuraAttributeSet->GetMaxMana();
	OnManaChanged.Broadcast(Mana);
	OnMaxManaChanged.Broadcast(MaxMana);


	// InitialValue - PlayerLevel
	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	const int32 CurrentPlayerLevel = AuraPlayerState->GetPlayerLevel();
	OnPlayerLevelChanged.Broadcast(CurrentPlayerLevel);

	// InitialValue - PlayerEXP
	const int32 CurrentEXP = AuraPlayerState->GetPlayerEXP();
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo.  Please fill out AuraPlayerState::LevelUpInfo in BP"));
	const int32 NowLevelUpRequirement = LevelUpInfo->GetLevelUpRequirement(CurrentPlayerLevel);
	const int32 PreviousLevelUpRequirement = LevelUpInfo->GetLevelUpRequirement(CurrentPlayerLevel - 1);
	const int32 DeltaLevelRequirement = NowLevelUpRequirement - PreviousLevelUpRequirement;
	const int32 EXPForThisLevel = CurrentEXP - PreviousLevelUpRequirement;
	const float EXPBarPercent = static_cast<float>(EXPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
	OnEXPPercentChanged.Broadcast(EXPBarPercent);
}

void UOverlayWidgetController::BindCallbackToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = GetAuraAS();
	AAuraPlayerState* AuraPlayerState = GetAuraPS();

	// GameplayAttributeValue Events
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

	// ASC Events
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	if (AuraASC != nullptr)
	{
		if (AuraASC->bStartupAbilitiesGiven)
			BroadcastAbilityInfo();
		else
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);

		AuraASC->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
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

	// Player's Level、EXP Events
	AuraPlayerState->OnPlayerEXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnPlayerEXPChanged);
	AuraPlayerState->OnPlayerLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChanged.Broadcast(NewLevel);
	});
}

void UOverlayWidgetController::OnPlayerEXPChanged(const int32 NewEXP)
{
	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo.  Please fill out AuraPlayerState::LevelUpInfo in BP"));

	const int32 NowLevel = LevelUpInfo->FindLevelForEXP(NewEXP);

	const int32 NowLevelUpRequirement = LevelUpInfo->GetLevelUpRequirement(NowLevel);
	const int32 PreviousLevelUpRequirement = LevelUpInfo->GetLevelUpRequirement(NowLevel - 1);

	const int32 DeltaLevelRequirement = NowLevelUpRequirement - PreviousLevelUpRequirement;
	const int32 EXPForThisLevel = NewEXP - PreviousLevelUpRequirement;

	const float EXPBarPercent = static_cast<float>(EXPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

	OnEXPPercentChanged.Broadcast(EXPBarPercent);
}
