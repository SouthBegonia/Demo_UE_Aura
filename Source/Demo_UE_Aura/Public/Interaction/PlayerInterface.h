// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMO_UE_AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForEXp(int32 InEXP) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetEXP();

	UFUNCTION(BlueprintNativeEvent)
	void AddToEXP(int32 InEXP);
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InLevel);
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InPoints);
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InPoints);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();


	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 InLevel) const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 InLevel) const;
};
