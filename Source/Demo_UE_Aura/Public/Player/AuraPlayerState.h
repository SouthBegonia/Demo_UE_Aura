

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32 /* StatValue */)

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region Level

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FOnPlayerStatChangedSignature OnPlayerEXPChangedDelegate;
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetPlayerEXP() const { return EXP; }

	void SetPlayerLevel(const int32 InLevel);
	void SetPlayerEXP(const int32 InEXP);
	FORCEINLINE void AddToLevel(const int32 InLevel) { SetPlayerLevel(GetPlayerLevel() + InLevel); }
	FORCEINLINE void AddToPlayerEXP(const int32 InEXP) { SetPlayerEXP(GetPlayerEXP() + InEXP); }

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	/*
	 * EXP
	 *	- Tip : value will always be increasing
	 */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_EXP)
	int32 EXP = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	UFUNCTION()
	void OnRep_EXP(int32 OldEXP);

#pragma endregion


#pragma region GAS

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

#pragma endregion

};
