

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|FireBlast")
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBlast")
	TSubclassOf<AAuraFireBall> FireBallClass;

#pragma region Ability Description

public:
	virtual FString GetAbilityDescription(int32 AbilityLevel) override;
	virtual FString GetNextAbilityDescription(int32 AbilityLevel) override;

#pragma endregion
};
