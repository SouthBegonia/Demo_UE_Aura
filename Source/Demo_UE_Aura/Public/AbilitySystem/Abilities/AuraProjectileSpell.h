

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Aura|Projectile")
	bool bDrawDebugLine = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/* Spawn a single Projectile to target location*/
	UFUNCTION(BlueprintCallable, Category="Aura|Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);


	void GenerateAndSpawnProjectile(const FVector& Location, const FRotator& Rotation) const;
};
