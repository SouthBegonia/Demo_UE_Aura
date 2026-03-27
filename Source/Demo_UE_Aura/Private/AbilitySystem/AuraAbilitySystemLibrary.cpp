


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetController(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetController(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetController(WorldContextObject, WidgetControllerParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	// Apply GE to initialize（Primary、Secondary、Vital）
	AActor* AvatarActor = ASC->GetAvatarActor();
	auto ApplyAttributesGE = [&ASC, &Level, &AvatarActor](const TSubclassOf<UGameplayEffect>& GameplayEffectClass)
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle AttributesSpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*AttributesSpecHandle.Data.Get());
	};
	ApplyAttributesGE(ClassDefaultInfo.PrimaryAttributes);
	ApplyAttributesGE(CharacterClassInfo->SecondaryAttributes);
	ApplyAttributesGE(CharacterClassInfo->VitalAttributes);
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
		return;

	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	// Apply GE [PrimaryAttributes_SetByCaller]
	const FGameplayEffectSpecHandle PrimaryAttributes_SpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller, 1, EffectContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributes_SpecHandle, Tags.Attributes_Primary_Strength, SaveGame->AS_Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributes_SpecHandle, Tags.Attributes_Primary_Intelligence, SaveGame->AS_Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributes_SpecHandle, Tags.Attributes_Primary_Resilience, SaveGame->AS_Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributes_SpecHandle, Tags.Attributes_Primary_Vigor, SaveGame->AS_Vigor);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributes_SpecHandle.Data);

	// Apply GE [PrimaryAttributes_SetByCaller]
	auto ApplyAttributesGE = [&ASC, &SourceAvatarActor](const TSubclassOf<UGameplayEffect>& GameplayEffectClass, const int32 Level)
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(SourceAvatarActor);
		const FGameplayEffectSpecHandle AttributesSpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*AttributesSpecHandle.Data.Get());
	};
	ApplyAttributesGE(CharacterClassInfo->SecondaryAttributes_Infinite, 1.f);
	ApplyAttributesGE(CharacterClassInfo->VitalAttributes, 1.f);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	// Give Common Abilities
	for (const TSubclassOf AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);	// this will not be affected by the PlayerLevel
		ASC->GiveAbility(GameplayAbilitySpec);
	}

	// Give Unique Abilities
	const FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
	if (CombatInterface)
	{
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterClassDefaultInfo.StartupAbilities)
		{
			FGameplayAbilitySpec GameplayAbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(GameplayAbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
		return nullptr;

	check(AuraGameMode->CharacterClassInfo)

	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr)
		return nullptr;

	check(AuraGameMode->AbilityInfo)

	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsBlockedHit() : false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsCriticalHit() : false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsSuccessfulDebuff() : false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->GetDebuffDamage() : 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->GetDebuffDuration() : 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->GetDebuffFrequency() : 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext != nullptr && AuraEffectContext->GetDamageType().IsValid())
	{
		return *AuraEffectContext->GetDamageType().Get();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext != nullptr)
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext != nullptr)
	{
		return AuraEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsRadialDamage() : false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->GetRadialDamageInnerRadius() : 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->GetRadialDamageOuterRadius() : 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	if (AuraEffectContext != nullptr)
	{
		return AuraEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlocked)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDamageType(MakeShared<FGameplayTag>(InDamageType));
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float SphereRadius,
                                                           const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(SphereRadius),
			SphereParams
		);

		for (const FOverlapResult& Overlap : Overlaps)
		{
			const bool bImplementsCombatInterface = Overlap.GetActor()->Implements<UCombatInterface>();
			if (bImplementsCombatInterface)
			{
				const bool bIsAlive = !ICombatInterface::Execute_IsDead(Overlap.GetActor());
				if (bIsAlive)
				{
					OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
				}
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*> Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0)
			break;

		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor = nullptr;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	check(FirstActor && SecondActor);

	auto CheckActorsHasSameTagFunc = [&FirstActor, &SecondActor](const FName& TagName)
	{
		return FirstActor->ActorHasTag(TagName) && SecondActor->ActorHasTag(TagName);
	};

	const bool bIsFriends = CheckActorsHasSameTagFunc(FName("Player")) || CheckActorsHasSameTagFunc(FName("Enemy"));

	return !bIsFriends;
}

int32 UAuraAbilitySystemLibrary::GetEXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
		return 0;

	const FCharacterClassDefaultInfo Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float EXPReward = Info.EXPReward.GetValueAtLevel(CharacterLevel);

	return FMath::FloorToInt32(EXPReward);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceASC->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	// Set Effect context
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceASC->MakeOutgoingSpec(DamageEffectParams.DamageEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.DamageTypesToDebuffTypeMap[DamageEffectParams.DamageType], 1.f);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Chance, DamageEffectParams.DebuffConfig.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Damage, DamageEffectParams.DebuffConfig.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Duration, DamageEffectParams.DebuffConfig.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Debuff_Frequency, DamageEffectParams.DebuffConfig.DebuffFrequency);

	// Apply GE
	DamageEffectParams.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;
}

bool UAuraAbilitySystemLibrary::MakeWidgetController(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.PlayerController = PC;
			OutWCParams.PlayerState = PS;
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;

			OutAuraHUD = AuraHUD;

			return true;
		}
	}

	return false;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	check(NumRotators > 0)

	TArray<FRotator> Rotators;

	if (NumRotators > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
		const FVector RightOfSpread = Forward.RotateAngleAxis(Spread / 2.f, Axis);
		const float DeltaSpread = Spread / (NumRotators - 1);

		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	check(NumVectors > 0)

	TArray<FVector> Vectors;

	if (NumVectors > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
		const FVector RightOfSpread = Forward.RotateAngleAxis(Spread / 2.f, Axis);
		const float DeltaSpread = Spread / (NumVectors - 1);

		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if (Magnitude == 0.f)
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	else
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if (Magnitude == 0.f)
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	else
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
}

void UAuraAbilitySystemLibrary::SetEffectParamsASC(FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetASC = InASC;
}
