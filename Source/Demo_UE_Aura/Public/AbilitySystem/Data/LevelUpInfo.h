

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT()
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	ULevelUpInfo();

	/*
	 * The Config of FAuraLevelUpInfo
	 *
	 * ATTENTION :
	 *	- The Index of Array is correspond to Level
	 *	- LevelUpInformation[0] is just a placeholder
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;

	int32 GetLevelUpRequirement(const int32 Level) const;
	FORCEINLINE int32 GetMaxLevel() const { return LevelUpInformation.Num() - 1; }

	int32 FindLevelForEXP(const int32 EXP) const;
};
