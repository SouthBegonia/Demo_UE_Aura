


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

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

	const UAbilitySystemComponent* SourceAbilitySystem = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAbilitySystem = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatarActor = SourceAbilitySystem ? SourceAbilitySystem->GetAvatarActor() : nullptr;
	AActor* TargetAvatarActor = TargetAbilitySystem ? TargetAbilitySystem->GetAvatarActor() : nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatarActor);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatarActor);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();


	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatarActor);
	check(CharacterClassInfo->DamageCalculationCoefficientsCurveTable)

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
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficientsCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	/*
	 * ArmorPenetration
	 *	- Usage : Ignores a percentage of the Target's Armor
	 *	- Value Source : CapturedAttributeMagnitude from Source
	 */
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration); // TODO : limit maximum is 100 ?
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficientsCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

#pragma endregion


#pragma region Core Calculation

	// Block : If block successful, halve the damage
	bool bIsBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	if (bIsBlocked)
	{
		Damage = Damage / 2.f;
	}

	// ArmorPenetration : Ignores a percentage of the Target's Armor. range of value [0, 100]
	const float TargetArmorEffective = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	// Armor : Ignores a percentage of incoming damage
	Damage = Damage * (100 - TargetArmorEffective * EffectiveArmorCoefficient) / 100.f;	 // TODO : limit TargetArmorEffective range of [0, 100] ?


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
