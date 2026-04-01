

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UAuraAbilitySystemComponent;
class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus : uint8
{
	Vacant = 0,
	EnterName = 1,
	Taken = 2,
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Default")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTypeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatusTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityInputTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;

	bool operator==(const FSavedAbility& Target) const
	{
		return AbilityTag.MatchesTagExact(Target.AbilityTag);
	}
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform ActorTransform = FTransform();

	// Serialized variables from the Actor - only those marked with saveGame specifier
	UPROPERTY()
	TArray<uint8> Bytes;

	bool operator==(const FSavedActor& Target) const
	{
		return ActorName == Target.ActorName;
	}
};

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

/*
 * FSaveGameModifiableParams
 * - using for InGame, not LoadMenu
 */
USTRUCT()
struct FSaveGameModifiableParams
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PlayerStartTag;
};

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

#pragma region SlotInfo

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = ESaveSlotStatus::Vacant;

	/* Use for initialize necessary data when first enter game with this SaveGame */
	UPROPERTY()
	bool bIsFirstTimeLoadIn = true;

#pragma endregion

#pragma region PlayerInfo

	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 PlayerEXP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;
	UPROPERTY()
	int32 AttributePoints = 0;

#pragma endregion

#pragma region AttributeInfo

	UPROPERTY()
	float AS_Strength = 0;
	UPROPERTY()
	float AS_Intelligence = 0;
	UPROPERTY()
	float AS_Resilience = 0;
	UPROPERTY()
	float AS_Vigor = 0;

#pragma endregion

#pragma region Abilities

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

#pragma endregion

#pragma region World/Map

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

public:
	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName) const;

#pragma endregion
};
