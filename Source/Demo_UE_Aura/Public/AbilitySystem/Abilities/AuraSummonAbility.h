

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Aura|Summon")
	bool bDrawDebugLine = false;

	UFUNCTION(BlueprintCallable, Category = "Aura|Summon")
	TArray<FVector> GetSpawnLocations();

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Summon")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Summon")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Summon")
	float MinSpawnDistance = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Summon")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Summon")
	float SpawnSpread = 90.f;
};
