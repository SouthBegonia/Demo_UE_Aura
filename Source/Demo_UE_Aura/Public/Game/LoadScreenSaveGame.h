

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum ESaveSlotStatus : uint8
{
	Vacant = 0,
	EnterName = 1,
	Taken = 2,
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
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = ESaveSlotStatus::Vacant;
};
