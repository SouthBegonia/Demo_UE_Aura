

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_VM_LoadScreen.generated.h"

class UMVVM_VM_LoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnySlotSelectedSignature);

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UMVVM_VM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	//UPROPERTY(BlueprintAssignable)
	//FAnySlotSelectedSignature AnySlotSelectedDelegate;

	UPROPERTY(EditDefaultsOnly, Category="Aura|UI")
	TSubclassOf<UMVVM_VM_LoadSlot> LoadSlotViewModelClass;


	UFUNCTION(BlueprintPure)
	UMVVM_VM_LoadSlot* GetLoadSlotViewModel(int32 Index) const;



	UFUNCTION(BlueprintCallable)
	void SlotButtonPressed_NewSlot(int32 SlotIndex, const FString& EnterName);
	UFUNCTION(BlueprintCallable)
	void SlotButtonPressed_NewGame(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
	void SlotButtonPressed_SelectSlot(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
	void SlotButtonPressed_Play(const TSoftObjectPtr<UWorld> Level);
	UFUNCTION(BlueprintCallable)
	void SlotButtonPressed_DeleteSlot();

	/* Load Data from SaveGame to initialize VM */
	void LoadAllSlotData();
	/* Load Data from SaveGame to initialize VM */
	void LoadTargetSlotData(int32 SlotIndex);

private:

	void SetSlotSelection(int32 SlotIndex);
	void ClearSlotSelection();

	UPROPERTY()
	TMap<int32, UMVVM_VM_LoadSlot*> LoadSlotsMap;

	UPROPERTY()
	TObjectPtr<UMVVM_VM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_VM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_VM_LoadSlot> LoadSlot_2;

	TWeakObjectPtr<UMVVM_VM_LoadSlot> CurrentSelectLoadSlot;

#pragma region Field Notifies

public:
	int32 GetCurrentSelectSlotIndex() const { return CurrentSelectSlotIndex; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, meta=(AllowPrivateAccess="true"))
	int32 CurrentSelectSlotIndex = -1;

#pragma endregion
};
