


#include "UI/Widget/AreYouSureWidget.h"

#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


void UAreYouSureWidget::SetShowParam(const FAreYouSureWidgetParam& InParam)
{
	// Clear register each time
	BtnOneClickDelegate.Clear();
	BtnTwoLClickDelegate.Clear();
	BtnTwoRClickDelegate.Clear();

	ShowParam = InParam;
}

void UAreYouSureWidget::RefreshShow(const FAreYouSureWidgetParam& Param)
{
	TextContextSwitcher->SetActiveWidgetIndex(Param.ContentText.IsEmpty() ? 1 : 0);
	BtnBoxSwitcher->SetActiveWidgetIndex(Param.bIsSingleBtn ? 1 : 0);

	if (!Param.TitleText.IsEmpty())
		Text_Title->SetText(FText::FromString(Param.TitleText));
	if (!Param.ContentText.IsEmpty())
		Text_Content->SetText(FText::FromString(Param.ContentText));
}

UAreYouSureWidget* UAreYouSureWidget::CreateAndShowAreYouSureWidget(const UObject* WorldContextObject, const TSubclassOf<UAreYouSureWidget> WidgetClass, const FAreYouSureWidgetParam& Param)
{
	check(WidgetClass != nullptr);

	UAreYouSureWidget* Widget = CreateWidget<UAreYouSureWidget>(WorldContextObject->GetWorld(), WidgetClass);
	Widget->SetShowParam(Param);
	// TODO : register BtnClickDelegate

	Widget->AddToViewport();

	return Widget;

	// you have to register BtnOneClickDelegate/BtnTwoLClickDelegate/BtnTwoRClickDelegate after create this widget,
	//		so that the delegates will invoke when click correct btn
}

void UAreYouSureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshShow(ShowParam);
}

void UAreYouSureWidget::NativeDestruct()
{
	Super::NativeDestruct();

	BtnOneClickDelegate.Clear();
	BtnTwoLClickDelegate.Clear();
	BtnTwoRClickDelegate.Clear();
}

void UAreYouSureWidget::OnBtnOneClicked()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s"), TEXT("OnBtnOneClicked")));
	BtnOneClickDelegate.Broadcast();

	RemoveFromParent();
}

void UAreYouSureWidget::OnBtnTwoLClicked()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s"), TEXT("OnBtnTwoLClicked")));
	BtnTwoLClickDelegate.Broadcast();

	RemoveFromParent();
}

void UAreYouSureWidget::OnBtnTwoRClicked()
{
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s"), TEXT("OnBtnTwoRClicked")));
	BtnTwoRClickDelegate.Broadcast();

	RemoveFromParent();
}
