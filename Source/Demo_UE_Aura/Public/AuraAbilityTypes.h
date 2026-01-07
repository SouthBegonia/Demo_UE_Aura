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
	UPROPERTY()
	TWeakObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> SourceASC;
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> TargetASC;

	UPROPERTY()
	int32 AbilityLevel = 1;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	UPROPERTY()
	float BaseDamage = 0.f;
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	FAbilityDebuffConfig DebuffConfig = FAbilityDebuffConfig();
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

	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(float bInDebuffDamage) { DebuffDamage = bInDebuffDamage; }
	void SetDebuffDuration(float bInDebuffDuration) { DebuffDuration = bInDebuffDuration; }
	void SetDebuffFrequency(float bInDebuffFrequency) { DebuffFrequency = bInDebuffFrequency; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }

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