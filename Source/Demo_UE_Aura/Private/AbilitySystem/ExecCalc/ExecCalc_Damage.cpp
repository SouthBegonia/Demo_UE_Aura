


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
	}
};
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#pragma region Get BaseInfo

	const UAbilitySystemComponent* TargetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAbilitySystem = ExecutionParams.GetSourceAbilitySystemComponent();

	const AActor* TargetActor = TargetAbilitySystem ? TargetAbilitySystem->GetAvatarActor() : nullptr;
	const AActor* SourceActor = SourceAbilitySystem ? SourceAbilitySystem->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

#pragma endregion


#pragma region Get Calculating Parameter

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;

	/*
	 * Damage
	 *	- Usage :
	 *	- Value Source : SetByCallerMagnitude
	 */
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	/*
	 * BlockChance
	 *	- Usage : If block successful, halve the damage
	 *	- Value Source : CapturedAttributeMagnitude from Target
	 */
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParams, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	/*
	 * Armor
	 *	- Usage : If block successful, halve the damage
	 *	- Value Source : CapturedAttributeMagnitude from Target
	 */
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParams, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor); // TODO : limit maximum is 100 ?

	/*
	 * ArmorPenetration
	 *	- Usage : Ignores a percentage of the Target's Armor
	 *	- Value Source : CapturedAttributeMagnitude from Source
	 */
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration); // TODO : limit maximum is 100 ?

#pragma endregion


#pragma region Core Calculation

	// Block : If block successful, halve the damage
	bool bIsBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	if (bIsBlocked)
	{
		Damage = Damage / 2.f;
	}

	// ArmorPenetration : Ignores a percentage of the Target's Armor. range of value [0, 100]
	const float TargetArmorEffective = TargetArmor * (100 - SourceArmorPenetration) / 100.f;
	// Armor : Ignores a percentage of incoming damage
	Damage = Damage * (100 - TargetArmorEffective) / 100.f;	 // TODO : limit TargetArmorEffective range of [0, 100] ?


#pragma endregion
	

#pragma region Apply Modifiers

	/*
	 * IncomingDamage
	 *	- Usage : will processing in UAuraAttributeSet::PostGameplayEffectExecute()
	 *	- Value Source : equal to Damage
	 */
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

#pragma endregion
}
