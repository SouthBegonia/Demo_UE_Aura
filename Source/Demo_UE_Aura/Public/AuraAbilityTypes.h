#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityDebuffConfig
{
	GENERATED_BODY()

	FAbilityDebuffConfig() {}

	/* DebuffType : Depends on DamageType */
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DebuffType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffChance = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffFrequency = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffDuration = 0.f;
};


USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {}

	/* The AvatarActor of causer */
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UAbilitySystemComponent> SourceASC;
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;

	UPROPERTY(BlueprintReadWrite)
	int32 AbilityLevel = 1;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	FAbilityDebuffConfig DebuffConfig = FAbilityDebuffConfig();

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayEffectContext::StaticStruct();
	}
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}


	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulse; }
	FVector GetKnockbackForce() const { return KnockbackForce; }

	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(float bInDebuffDamage) { DebuffDamage = bInDebuffDamage; }
	void SetDebuffDuration(float bInDebuffDuration) { DebuffDuration = bInDebuffDuration; }
	void SetDebuffFrequency(float bInDebuffFrequency) { DebuffFrequency = bInDebuffFrequency; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
	void SetKnockbackForce(const FVector& InKnockbackForce) { KnockbackForce = InKnockbackForce; }

protected:

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;

	TSharedPtr<FGameplayTag> DamageType;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};