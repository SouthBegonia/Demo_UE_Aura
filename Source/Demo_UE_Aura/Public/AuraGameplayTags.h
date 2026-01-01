

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/*
 * AuraGameplayTags
 *		- Singleton containing native GameplayTags
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

#pragma region Attributes

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;


	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;


	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lighting;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	FGameplayTag Attributes_Meta_IncomingEXP;

#pragma endregion


#pragma region InputTag

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

#pragma endregion


#pragma region Damage

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lighting;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	/*
	 * Map of Damage Types to Resistances
	 *	- ex: key = Damage_Fire, value = Attributes_Resistance_Fire
	 */
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistancesMap;

#pragma endregion


#pragma region Abilities

	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_HitReact;

	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Lightning_Electrocute;

	/*
	 * Locked : The status of an ability that have not yet met the unlocking conditions
	 */
	FGameplayTag Abilities_Status_Locked;
	/*
	 * Eligible : The status of an ability that meets the unlocking conditions (such as the player's level) but has not been actively unlocked yet
	 */
	FGameplayTag Abilities_Status_Eligible;
	/*
	 * Unlocked : The status of an ability that has been actively unlocked
	 */
	FGameplayTag Abilities_Status_Unlocked;
	/*
	 * Equipped : The status of an ability that has been equipped and can be activated
	 */
	FGameplayTag Abilities_Status_Equipped;


	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;


#pragma endregion


#pragma region Cooldown

	FGameplayTag Cooldown_Fire_FireBolt;

#pragma endregion


#pragma region CombatSocket

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;

#pragma endregion


#pragma region Montage

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

#pragma endregion


#pragma region Effects

	FGameplayTag Effects_HitReact;

#pragma endregion


private:
	static FAuraGameplayTags GameplayTags;
};
