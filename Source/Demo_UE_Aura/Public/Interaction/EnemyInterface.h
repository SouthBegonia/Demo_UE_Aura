

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum EEnemyDebugState : uint8
{
	None,

	Idle,	// Do nothing
};

/**
 * 
 */
class DEMO_UE_AURA_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool InDebugState();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool InInDebugState(EEnemyDebugState TargetState);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EEnemyDebugState GetDebugState();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);
};
