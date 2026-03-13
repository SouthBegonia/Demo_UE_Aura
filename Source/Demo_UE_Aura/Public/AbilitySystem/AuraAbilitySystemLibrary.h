

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraAbilitySystemLibrary.generated.h"


class AAuraHUD;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

#pragma region Widget Controller

public:
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

private:
	static bool MakeWidgetController(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD);

#pragma endregion


#pragma region Ability System Class Defaults

public:
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|CharacterClassDefault")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|CharacterClassDefault")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|CharacterClassDefault")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

#pragma endregion


#pragma region Effect Context Getter/Setter

public:
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlocked);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);
	UFUNCTION(BlueprintPure, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);

#pragma endregion


#pragma region Gameplay Machanics

public:
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float SphereRadius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*> Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Aura|AbilitySystem|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Aura|AbilitySystem|GameplayMechanics")
	static int32 GetEXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);

#pragma endregion


#pragma region Damage Effect Params

public:
	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|DamageEffect")
	static void SetIsRadialDamageEffectParam(UPARAM(ref) FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OuterRadius, FVector Origin);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|DamageEffect")
	static void SetKnockbackDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude = 0.f);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|DamageEffect")
	static void SetDeathImpulseDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude = 0.f);

	UFUNCTION(BlueprintCallable, Category="Aura|AbilitySystem|DamageEffect")
	static void SetEffectParamsASC(UPARAM(ref) FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC);

#pragma endregion
};
