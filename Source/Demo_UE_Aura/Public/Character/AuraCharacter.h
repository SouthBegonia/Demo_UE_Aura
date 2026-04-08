

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	virtual int32 GetPlayerLevel_Implementation() override;

	virtual void OnRep_Burned() override;
	virtual void OnRep_Stunned() override;

protected:
	void LoadProgress();

#pragma region PlayerInterface
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;

	virtual int32 GetEXP_Implementation() override;

	virtual void AddToEXP_Implementation(int32 InEXP) override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InPoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InPoints) override;
	virtual void LevelUp_Implementation() override;

	virtual int32 FindLevelForEXp_Implementation(int32 InEXP) const override;

	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;

	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;

	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) const override;
	virtual void HideMagicCircle_Implementation() const override;

	virtual bool SaveProgress_Implementation(const FName& CheckpointTag) override;
	virtual bool SaveProgressWithParams_Implementation(FSaveGameModifiableParams& ModifyParams) override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

#pragma endregion

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
