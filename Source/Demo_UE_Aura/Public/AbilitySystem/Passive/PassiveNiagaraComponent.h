

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UPassiveNiagaraComponent();

	UPROPERTY(VisibleAnywhere, Category="Aura")
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;

	void ActivateIfEquipped(class UAuraAbilitySystemComponent* AuraASC);

	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
};
