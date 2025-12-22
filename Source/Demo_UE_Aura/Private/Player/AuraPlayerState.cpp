


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);		//NetUpdateFrequency = 100.f;


	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, EXP);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

#pragma region Level

void AAuraPlayerState::SetPlayerEXP(const int32 InEXP)
{
	check(InEXP >= 0)
	const bool bChanged = EXP != InEXP;

	EXP = InEXP;

	if (bChanged)
		OnPlayerEXPChangedDelegate.Broadcast(EXP);
}

void AAuraPlayerState::SetPlayerLevel(const int32 InLevel)
{
	check(InLevel > 0)
	const bool bChanged = Level != InLevel;

	Level = InLevel;

	if (bChanged)
		OnPlayerLevelChangedDelegate.Broadcast(Level);
}


void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_EXP(int32 OldEXP)
{
	OnPlayerEXPChangedDelegate.Broadcast(EXP);
}

#pragma endregion