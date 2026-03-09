


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightingResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)

	/*
	 * Map of Attribute's GameplayTag to FGameplayEffectAttributeCaptureDefinition
	 *	- ex: key = Attributes_Secondary_Armor, value = ArmorDef
	 */
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefsMap;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightingResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false)


		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);

		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Resistance_Lighting, LightingResistanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefsMap.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
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
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightingResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
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

	const int32 SourcePlayerLevel = SourceAvatarActor->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(SourceAvatarActor) : 1;
	const int32 TargetPlayerLevel = TargetAvatarActor->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(TargetAvatarActor) : 1;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	//FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	//FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext);

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
	 *	- Usage : Comprehensive total damage value
	 *	- Value Source :
	 */
	float Damage = 0.f;
	/*
	 * DamageMap
	 *	- Usage : Record the damage values of different types
	 *	- Value Source : SetByCallerMagnitude
	*	- Key = GameplayTag to DamageType, Value = Damage value of target type
	 */
	TMap<FGameplayTag, float> DamageMap;
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistancesMap)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		//const FGameplayTag DamageResistanceAttributeTag = Pair.Value;
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0);

		// Record DamageValue of Target DamageType
		DamageMap.Add(DamageTypeTag, DamageTypeValue);
	}
	/*
	 * TargetDamageResistanceMap
	 *	- Usage : Record the damage values of different types
	 *	- Value Source : CapturedAttributeMagnitude from Target
	 *	- Key = GameplayTag to ResistanceAttribute, Value = Value of ResistanceAttribute
	 */
	TMap<FGameplayTag, float> TargetDamageResistanceMap;
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistancesMap)
	{
		//const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag DamageResistanceAttributeTag = Pair.Value;

		checkf(AuraDamageStatics().TagsToCaptureDefsMap.Contains(DamageResistanceAttributeTag), TEXT("TagsToCaptureDefsMap doesn't contain Tag=[%s] is ExecCalc_Damage"), *DamageResistanceAttributeTag.ToString())
		const FGameplayEffectAttributeCaptureDefinition CaptureAttributeDef = AuraDamageStatics().TagsToCaptureDefsMap[DamageResistanceAttributeTag];
		float ResistanceValue = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureAttributeDef, EvaluateParams, ResistanceValue);
		ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100.f);

		// Record ResistanceAttributeValue of Target DamageType
		TargetDamageResistanceMap.Add(DamageResistanceAttributeTag, ResistanceValue);
	}

	/*
	 * TargetDebuffMap
	 *	- Usage : Record the DebuffConfig(ex. DebuffChance、DebuffDamage...) of different damage types
	*	- Value Source : SetByCallerMagnitude
	*	- Key = GameplayTag to ResistanceAttribute, Value = FAbilityDebuffConfig
	*
	*	- [ATTENTION] : There might be no one or at most one effective DebuffData from Spec,
	*					Because only one DebuffTypeTag is passed on each Execute_Implementation. (see UAuraAbilitySystemLibrary::ApplyDamageEffect())
	 */
	TMap<FGameplayTag, FAbilityDebuffConfig> TargetDebuffMap;
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToDebuffTypeMap)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag DebuffTypeTag = Pair.Value;
		const FGameplayTag DamageResistanceAttributeTag = FAuraGameplayTags::Get().DamageTypesToResistancesMap[DamageTypeTag];

		FAbilityDebuffConfig DebuffConfig;
		if (Spec.GetSetByCallerMagnitude(DebuffTypeTag, false, -1.f) > 0.f)
		{
			DebuffConfig.DebuffType = DebuffTypeTag;
			DebuffConfig.DebuffChance = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Debuff_Chance, false, -1.f);
			DebuffConfig.DebuffDamage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Debuff_Damage, false, -1.f);
			DebuffConfig.DebuffFrequency = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Debuff_Frequency, false, -1.f);
			DebuffConfig.DebuffDuration = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Debuff_Duration, false, -1.f);
		}
		else
		{
			// this not the corresponding debuff type in Spec
			DebuffConfig.DebuffType = FGameplayTag();
			DebuffConfig.DebuffChance = -1.f;
			DebuffConfig.DebuffDamage = -1.f;
			DebuffConfig.DebuffFrequency = -1.f;
			DebuffConfig.DebuffDuration = -1.f;
		}

		TargetDebuffMap.Add(DamageResistanceAttributeTag, DebuffConfig);
	}


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
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	/*
	 * ArmorPenetration
	 *	- Usage : Ignores a percentage of the Target's Armor
	 *	- Value Source : CapturedAttributeMagnitude from Source
	 */
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration); // TODO : limit maximum is 100 ?
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficientsCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	/*
	 * CriticalHitChance
	 *	- Usage : Chance to double damage
	 *	- Value Source : CapturedAttributeMagnitude from Source
	 */
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);
	/*
	 * CriticalHitDamage
	 *	- Usage : Bonus damage added when a Critical Hit is scored
	 *	- Value Source : CapturedAttributeMagnitude from Source
	 */
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParams, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	/*
	 * CriticalHitResistance
	 *	- Usage : Reduces CriticalHitChance
	 *	- Value Source : CapturedAttributeMagnitude from Target
	 */
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParams, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficientsCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);


#pragma endregion


#pragma region Core Calculation

	// Radial Damage : If was Radial Damage, reduce damage in phases
	// NOTE : this will modify DamageMap(which record the original DamageValue from SetByCallerMagnitude)
	for (TPair<FGameplayTag, float>& Pair : DamageMap)
	{
		float DamageTypeValueOriginal = Pair.Value;

		if (DamageTypeValueOriginal > 0.f && UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			// 1. Override TakeDamage in AuraCharacterBase
			// 2. Create delegate OnDamageDelegate, broadcast damage received in TakeDamage
			// 3. Bind lambda to OnDamageDelegate on the Victim here
			// 4. Call UGameplayStatics::ApplyRadialDamageWithFalloff to cause damage(this will result in TakeDamage being called on the Victim, which will then broadcast OnDamageDelegate)
			// 5. In Lambda, set DamageTypeValue to the damage received from the broadcast
			if (TargetCombatInterface)
			{
				FDelegateHandle handle = TargetCombatInterface->GetOnDamageDelegate().AddLambda([&](float DamageAmount)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("DamageTypeValue = %f, DamageAmount = %f"), Pair.Value, DamageAmount));
					Pair.Value = DamageAmount;

					if (handle.IsValid())
						TargetCombatInterface->GetOnDamageDelegate().Remove(handle);	// TODO : Find a better way to unbind event.
				});
			}
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatarActor,
				DamageTypeValueOriginal,
				0.f,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatarActor,
				nullptr
			);
		}
	}


	// Damage Resistance : Reduce the same type damage by Resistance
	for (const TPair<FGameplayTag, float>& Pair : DamageMap)
	{
		const float DamageTypeValueOriginal = Pair.Value;
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag DamageTypeResistanceAttributeTag = FAuraGameplayTags::Get().DamageTypesToResistancesMap[DamageTypeTag];

		const float DamageTypeResistanceValue = TargetDamageResistanceMap[DamageTypeResistanceAttributeTag];
		float DamageTypeValue = DamageTypeValueOriginal * ((100.f - DamageTypeResistanceValue) / 100.f);	// Effective Damage value of this type

		Damage += DamageTypeValue;
	}

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

	//  CriticalHit : If CriticalHit, Double damage
	//  CriticalHitDamage : Bonus damage added when CriticalHit
	const float CriticalHitChanceEffective = SourceCriticalHitChance - (TargetCriticalHitResistance * CriticalHitResistanceCoefficient);
	const bool bCriticalHit = FMath::RandRange(1, 100) < CriticalHitChanceEffective;
	if (bCriticalHit)
	{
		Damage = 2.f * Damage;
		Damage = Damage + SourceCriticalHitDamage;
		// TODO : is CriticalHitResistance use for Reduces CriticalHitChance or Reduces CriticalHitDamage?
	}

	// Debuff : Probabilistic trigger. Once triggered, it will cause debuff damage
	bool bTriggerDebuff = false;
	FAbilityDebuffConfig TriggeredDebuffConfig;
	for (const TPair<FGameplayTag, FAbilityDebuffConfig>& DebuffMap : TargetDebuffMap)
	{
		const FGameplayTag& DamageTypeResistanceAttributeTag = DebuffMap.Key;
		const FAbilityDebuffConfig& DebuffConfig = DebuffMap.Value;

		if (DebuffConfig.DebuffType.IsValid() && DebuffConfig.DebuffChance > -.5f)	// It has the possibility to trigger Debuff
		{
			const float DamageTypeResistanceValue = TargetDamageResistanceMap[DamageTypeResistanceAttributeTag];
			const float EffectiveDebuffChance = DebuffConfig.DebuffChance * (100 - DamageTypeResistanceValue) / 100.f;

			bTriggerDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bTriggerDebuff)	// Trigger Debuff Successful
			{
				TriggeredDebuffConfig = DebuffConfig;
			}
		}
	}

#pragma endregion
	


#pragma region Customized EffectContext

	// Block
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bIsBlocked);

	// CriticalHit
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// Debuff
	if (bTriggerDebuff)
	{
		UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, true);

		const FGameplayTag DamageType = *FAuraGameplayTags::Get().DamageTypesToDebuffTypeMap.FindKey(TriggeredDebuffConfig.DebuffType);
		UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageType);
		UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, TriggeredDebuffConfig.DebuffDamage);
		UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, TriggeredDebuffConfig.DebuffDuration);
		UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, TriggeredDebuffConfig.DebuffFrequency);
	}


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
