

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class ULoadScreenWidget;
class UMVVM_VM_LoadScreen;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|UI")
	TSubclassOf<UUserWidget> LoadScreenWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	UPROPERTY(EditDefaultsOnly, Category="Aura|UI")
	TSubclassOf<UMVVM_VM_LoadScreen> LoadScreenViewModelClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_VM_LoadScreen> LoadScreenViewModel;

protected:
	virtual void BeginPlay() override;
};
