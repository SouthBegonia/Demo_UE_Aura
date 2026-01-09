

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	int32 MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float HomingAccelerationMax = 3200.f;

private:
	/* Only for Debug, get truly NumProjectiles by GetNumProjectiles()  */
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	int32 DebugNumProjectiles = -1;

protected:
	UFUNCTION(BlueprintCallable, Category="Aura|FireBolt")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f, AActor* HomingTargetActor = nullptr);

	FORCEINLINE int32 GetNumProjectiles() const { return FMath::Min(MaxNumProjectiles, GetAbilityLevel()); }

#pragma region Ability Description

public:
	virtual FString GetAbilityDescription(int32 AbilityLevel) override;
	virtual FString GetNextAbilityDescription(int32 AbilityLevel) override;

#pragma endregion
};
