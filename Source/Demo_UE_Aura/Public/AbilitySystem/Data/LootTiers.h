

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FlootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|lootTiers|Spawning")
	TSubclassOf<AActor> LootItemClass;

	UPROPERTY(EditAnywhere, Category="Aura|lootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category="Aura|lootTiers|Spawning")
	int32 MaxNumberToSpawn = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|lootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|lootTiers|Spawning")
	TArray<FlootItem> LootItems;

	UFUNCTION(BlueprintCallable)
	TArray<FlootItem> GetLootItems();
};
