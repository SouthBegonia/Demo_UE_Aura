

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AreYouSureWidget.generated.h"

class UWidgetSwitcher;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAreYouSureBtnClickSignatue);

USTRUCT(BlueprintType)
struct FAreYouSureWidgetParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsSingleBtn = false;

	UPROPERTY(BlueprintReadWrite)
	FString TitleText = TEXT("Are You Sure?");
	UPROPERTY(BlueprintReadWrite)
	FString ContentText = TEXT("");

	UPROPERTY(BlueprintReadWrite)
	FString BtnOneText = TEXT("OK");
	UPROPERTY(BlueprintReadWrite)
	FString BtnTwoLText = TEXT("OK");
	UPROPERTY(BlueprintReadWrite)
	FString BtnTwoRText = TEXT("CANCEL");
};

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAreYouSureWidget : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RefreshShow(const FAreYouSureWidgetParam& param);

	UFUNCTION(BlueprintCallable, Category="Aura|UI", meta=(DefaultToSelf = "WorldContextObject", ToolTip="[Create and show ShowAreYouSureWidget]\n- Setting Param to control display mode and display content.\n- Bind Button's delegate by BtnOneClickDelegate/BtnTwoLClickDelegate/BtnTwoRClickDelegate"))
	static UAreYouSureWidget* CreateAndShowAreYouSureWidget(const UObject* WorldContextObject, TSubclassOf<UAreYouSureWidget> WidgetClass, const FAreYouSureWidgetParam& Param);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetShowParam(const FAreYouSureWidgetParam& InParam);

	UFUNCTION(BlueprintCallable)
	void OnBtnOneClicked();
	UFUNCTION(BlueprintCallable)
	void OnBtnTwoLClicked();
	UFUNCTION(BlueprintCallable)
	void OnBtnTwoRClicked();

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Content;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> TextContextSwitcher;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> BtnBoxSwitcher;

	UFUNCTION(BlueprintImplementableEvent)
	void SetOneBtnLText(const FString& InText);
	UFUNCTION(BlueprintImplementableEvent)
	void SetTwoBtnLText(const FString& InText);
	UFUNCTION(BlueprintImplementableEvent)
	void SetTwoBtnRText(const FString& InText);

private:
	FAreYouSureWidgetParam ShowParam;

	UPROPERTY(BlueprintAssignable, Category="Aura|UI|AreYouSureWidget")
	FAreYouSureBtnClickSignatue BtnOneClickDelegate;
	UPROPERTY(BlueprintAssignable, Category="Aura|UI|AreYouSureWidget")
	FAreYouSureBtnClickSignatue BtnTwoLClickDelegate;
	UPROPERTY(BlueprintAssignable, Category="Aura|UI|AreYouSureWidget")
	FAreYouSureBtnClickSignatue BtnTwoRClickDelegate;
};
