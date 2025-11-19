

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {};

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;


	TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToAttributes;


#pragma region Attributes - Vital

	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Health, Category="Attributes|Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Mana, Category="Attributes|Vital")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

#pragma endregion

#pragma region Attributes - Primary

	/* Strength : Increases physical damage */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Strength, Category="Attributes|Primary")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	/* Intelligence : Increases magical damage */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Intelligence, Category="Attributes|Primary")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	/* Resilience : Increases armor and armor penetration */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Resilience, Category="Attributes|Primary")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	/* Vigor : Increases Health */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Vigor, Category="Attributes|Primary")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

#pragma endregion

#pragma region Attributes - Secondary

	/**
	 * Armor : Reduces damage taken, improves @BlockChance
	 *  - depends on @Resilience
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_Armor, Category="Attributes|Secondary")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	/**
	 * ArmorPenetration : Ignores percentage of enemy @Armor, increases @CriticalHitChance
	 *  - depends on @Resilience
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_ArmorPenetration, Category="Attributes|Secondary")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	/**
	 * BlockChance : Chance to cut incoming damage in half
	 *  - depends on @Armor
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_BlockChance, Category="Attributes|Secondary")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	/**
	 * CriticalHitChance : Chance to double damage plus Critical Hit bonus
	 *  - depends on @ArmorPenetration
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_CriticalHitChance, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	/**
	 * CriticalHitDamage : Bonus damage added when a Critical Hit is scored
	 *  - depends on @ArmorPenetration
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_CriticalHitDamage, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	/**
	 * CriticalHitResistance : Reduces @CriticalHitDamage of attacking enemies
	 *  - depends on @Armor
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_CriticalHitResistance, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	/**
	 * HealthRegeneration : Amount of @Health regenerated every 1 second
	 *  - depends on @Vigor
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_HealthRegeneration, Category="Attributes|Secondary")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	/**
	 * ManaRegeneration : Amount of @Mana regenerated every 1 second
	 *  - depends on @Intelligence
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_ManaRegeneration, Category="Attributes|Secondary")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	/**
	 * MaxHealth
	 *  - depends on @Vigor
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_MaxHealth, Category="Attributes|Secondary")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	/**
	 * MaxMana
	 *  - depends on @Intelligence
	 */
	UPROPERTY(BlueprintReadOnly, Replicated = OnRep_MaxMana, Category="Attributes|Secondary")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

#pragma endregion

#pragma region Attributes - Meta

	UPROPERTY(BlueprintReadOnly, /*Replicated = OnRep_Health,*/ Category="Attributes|Meta")	//do not Replicate, only process on Server
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);

#pragma endregion

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	void ShowFloatingText(const FEffectProperties& Props, float Damage) const;
};
