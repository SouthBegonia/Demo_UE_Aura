

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"


class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	float BaseWalkSpeed = 600.f;

#pragma region Combat

public:
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse = FVector::ZeroVector);

	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly, Category="Aura|Combat")
	bool bIsBurned = false;
	UFUNCTION()
	virtual void OnRep_Burned();

	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly, Category="Aura|Combat")
	bool bIsStunned = false;
	UFUNCTION()
	virtual void OnRep_Stunned();

	/* Is character under Lighting Attack, then will play special HitReact Montage */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Aura|Combat")
	bool bIsBeingShocked = false;

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	FORCEINLINE void SetCharacterClass(const ECharacterClass InCharacterClass) { CharacterClass = InCharacterClass; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

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
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;

	UPROPERTY(BlueprintReadOnly, Category="Aura|Combat")
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


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffNiagaraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffNiagaraComponent;

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


#pragma region Combat - PassiveAbility

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

#pragma endregion


#pragma region GAS

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FOnASCRegistered OnAscRegisteredDelegate;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;

	FOnDeathSignature OnDeathDelegate;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }


	FOnDamageSignature OnDamageDelegate;
	virtual FOnDamageSignature& GetOnDamageDelegate() override;

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
	virtual void InitializeSavedGameAttributes(ULoadScreenSaveGame& SaveGame);
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float Level) const;


	void AddCharacterAbilities();

private:
	UPROPERTY(EditAnywhere, Category="Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

#pragma endregion

};
