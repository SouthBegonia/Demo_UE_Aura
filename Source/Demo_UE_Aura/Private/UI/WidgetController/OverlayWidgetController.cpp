


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

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
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);

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

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (AuraASC != nullptr)
	{
		if (AuraASC->bStartupAbilitiesGiven)
			OnInitializeStartupAbilities(AuraASC);
		else
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);

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

	AuraPlayerState->OnPlayerEXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnPlayerEXPChanged);
	AuraPlayerState->OnPlayerLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChanged.Broadcast(NewLevel);
	});
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	// Get AbilityInformation from ASC after StartupAbilitiesGiven, and broadcast it to Widget
	if (!AuraASC->bStartupAbilitiesGiven)
		return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, &AuraASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		// execute on each Activatable Ability

		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AuraASC->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);

		// Broadcast AbilityInfo to WidgetController
		AbilityInfoDelegate.Broadcast(Info);
	});

	AuraASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnPlayerEXPChanged(const int32 NewEXP) const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
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
