

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"


class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class DEMO_UE_AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

protected:
	virtual void BeginPlay() override;

#pragma region Combat

public:
	virtual void Die() override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	TArray<FTaggedMontage> AttackMontages;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	FName TailSocketName;

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;

	bool bDead = false;


	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	virtual UAnimMontage* GetHitReactMontage_Implementation() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<USoundBase> DeathSound;


#pragma endregion

#pragma region Combat - Summon

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat", meta = (ToolTip="(Only for Ability that can summon) The maximum minion count that can be summoned"))
	int32 MaxMinionCount = 0;

	// The total count of minion that have been summoned
	int32 MinionCount = 0;

	virtual int32 GetMaxMinionCount_Implementation() override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual bool CanSummonMinion_Implementation() override;
	virtual void IncremenetMinionCount_Implementation(const int32 Amount) override;

#pragma endregion


#pragma region GAS

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Aura|Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	virtual void InitializeDefaultAttributes() const;
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float Level) const;


	void AddCharacterAbilities();

private:
	UPROPERTY(EditAnywhere, Category="Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

#pragma endregion

};
