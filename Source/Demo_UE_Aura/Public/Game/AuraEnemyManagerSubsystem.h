

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Subsystems/WorldSubsystem.h"
#include "AuraEnemyManagerSubsystem.generated.h"

class AAuraEnemy;

#define SpawnedEnemyCodeStartIndex 100

DECLARE_DELEGATE_OneParam(FForEachAuraEnemy, AAuraEnemy*);

USTRUCT(BlueprintType)
struct FAuraSpawnEnemyParameters
{
	GENERATED_BODY()


	FAuraSpawnEnemyParameters() {};
	FAuraSpawnEnemyParameters(const TSubclassOf<AAuraEnemy>& InEnemyClass, const int32 InEnemyLevel, const ECharacterClass& InEnemyCharacterClass) :
		EnemyClass(InEnemyClass),
		EnemyLevel(InEnemyLevel),
		EnemyCharacterClass(InEnemyCharacterClass)
	{

	}

public:
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(BlueprintReadWrite)
	int32 EnemyLevel = 1;

	UPROPERTY(BlueprintReadWrite)
	ECharacterClass EnemyCharacterClass = ECharacterClass::Warrior;


	UPROPERTY(BlueprintReadWrite)
	FTransform SpawnTransform = FTransform();
};


UCLASS(Blueprintable)
class DEMO_UE_AURA_API UAuraEnemyManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf = "WorldContextObject"))
	AAuraEnemy* TrySpawnAuraEnemy(const UObject* WorldContextObject, const FAuraSpawnEnemyParameters& SpawnParams);

	UFUNCTION()
	void HandleAuraEnemyDied(int32 EnemyCode);

public:
	void ForEachAuraEnemyInWorld(const FForEachAuraEnemy& Delegate) const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	bool bSystemEnabled = false;


	int32 SpawnedEnemyCodeIndex = SpawnedEnemyCodeStartIndex;
	TSet<int32> SpawnedEnemyCodeSet;
};
