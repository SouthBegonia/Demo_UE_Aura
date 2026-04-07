

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AAuraEnemy* SpawnEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	int32 EnemyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
};
