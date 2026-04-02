

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	void ShockLoopTagChanged(const bool bInShockLoop);



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Character Class Defaults")
	int32 Level = 1;


#pragma region Combat

public:
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Aura|Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category="Aura|Combat")
	TWeakObjectPtr<AActor> CombatTarget;

#pragma endregion


#pragma region CursorHighlight

public:
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

#pragma endregion


#pragma region UI

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

#pragma endregion


#pragma region AI

protected:
	UPROPERTY(EditAnywhere, Category="Aura|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

#pragma endregion

#pragma region Debug

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|DEBUG")
	TEnumAsByte<EEnemyDebugState> EnemyDebugState = EEnemyDebugState::None;

	virtual bool InDebugState_Implementation() override;
	virtual bool InInDebugState_Implementation(EEnemyDebugState TargetState) override;
	virtual EEnemyDebugState GetDebugState_Implementation() override;

#pragma endregion
};
