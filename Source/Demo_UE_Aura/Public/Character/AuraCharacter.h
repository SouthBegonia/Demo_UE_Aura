

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	virtual int32 GetPlayerLevel() override;

#pragma region GAS

private:
	virtual void InitAbilityActorInfo() override;

#pragma endregion

#pragma region Camera

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> CameraComp;

#pragma endregion

#pragma region HUD

private:
	void InitHUD();

#pragma endregion

};
