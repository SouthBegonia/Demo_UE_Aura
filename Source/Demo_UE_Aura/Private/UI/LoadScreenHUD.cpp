


#include "UI/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_VM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create LoadScreenVM + LoadScreenWidget
	LoadScreenViewModel = NewObject<UMVVM_VM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	LoadScreenWidget->BlueprintInitializeWidget();

	// Load LoadScreenVMData (data from GameSave)
	LoadScreenViewModel->LoadAllSlotData();


	// Set InputMode
	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GetOwningPlayerController()->SetInputMode(InputModeData);
	GetOwningPlayerController()->SetShowMouseCursor(true);
}
