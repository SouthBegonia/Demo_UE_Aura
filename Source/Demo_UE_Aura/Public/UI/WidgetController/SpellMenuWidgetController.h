

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DEMO_UE_AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Aura|GAS|Attributes")
	FOnPlayerStatChangeSignature SpellPointsChangedDelegate;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbackToDependencies() override;
};
