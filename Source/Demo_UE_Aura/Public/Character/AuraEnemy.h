

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

#pragma region Level

public:
	virtual int32 GetPlayerLevel() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Character Class Defaults")
	int32 Level = 1;

#pragma endregion


#pragma region CursorHighlight

public:
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

#pragma endregion

};
