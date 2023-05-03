/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#pragma once

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "UI/BaseWidgets/ModioLoadingSpinner.h"
#include "UI/Dialogs/ModioDialogInfo.h"
#include "UI/Interfaces/IModioInputMappingAccessor.h"
#include "UI/Styles/ModioUIStyleRef.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Views/STileView.h"

#include "ModioDialogBaseInternal.generated.h"
/**
 *
 */
UCLASS()
class MODIOUI_API UModioDialogBaseInternal : public UWidget, public IModioUIInputHandler
{
	GENERATED_BODY()

protected:
	TArray<TSharedPtr<struct FModioDialogButtonInfo>> ButtonParams;
	bool bBeingPreviewed = false;
	bool bUsingCustomButtons = false;
	SOverlay::FOverlaySlot* DialogContentOverlaySlot;
	SGridPanel::FSlot* InputWidgetSlot;
	SGridPanel::FSlot* ButtonWidgetSlot;
	SGridPanel::FSlot* SubHeaderWidgetSlot;
	SGridPanel::FSlot* DialogTextBlockSlot;
	SOverlay::FOverlaySlot* LoadingSpinnerOverlaySlot;
	TSharedPtr<SOverlay> DialogInternalRoot;
	TSharedPtr<class SImage> BackgroundImage;
	TSharedPtr<class SGridPanel> ContentPanel;
	TSharedPtr<class SModioRichTextBlock> HeadingTextBlock;
	TSharedPtr<class SModioRichTextBlock> DialogTextBlock;
	TSharedPtr<class STileView<TSharedPtr<FModioDialogButtonInfo>>> DialogButtons;

	UPROPERTY()
	UWidget* LoadingSpinner;

	UPROPERTY()
	UObject* DataSource;

	bool IsInputValid();

	virtual void BuildCommandList(TSharedRef<FUICommandList> InCommandList) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (StyleClass = "ModioDialogStyle"), Category = "Widgets")
	FModioUIStyleRef DialogStyle = {FName("DefaultDialogStyle")};
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets")
	FMargin ButtonSpacing;

	UPROPERTY()
	UWidget* SubHeaderWidget;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	FReply OnButtonClicked(TSharedPtr<FModioDialogButtonInfo> Button);
	void ApplyStyling();
	void UpdateBoundValues(class UModioDialogInfo* DialogDescriptor);
	EVisibility GetButtonListVisibility() const;
	TSharedRef<class ITableRow> OnGenerateButton(TSharedPtr<struct FModioDialogButtonInfo> ButtonInfo,
												 const TSharedRef<class STableViewBase>& OwnerTableView);
	const FModioUIStyleRef* GetButtonTextStyle() const;

	UFUNCTION()
	void OnNavigateDownFromCodeInputWidget();

	UFUNCTION()
	void OnSubmitKeyPressed();

	UPROPERTY()
	TWeakObjectPtr<class UModioDialogController> Controller;

	FCustomWidgetNavigationDelegate NavDelegate;

public:
	UPROPERTY()
	UWidget* InputWidget;

	UPROPERTY()
	UWidget* ButtonWidget;

	TArray<TSharedPtr<SWidget>> GeneratedButtons;
	TArray<TSharedPtr<struct FModioDialogButtonInfo>> ButtonInfos;

	void ShowLoadingSpinner();
	void HideLoadingSpinner();

	void SetDialogController(UModioDialogController* DialogController);
	void InitializeFromDialogInfo(class UModioDialogInfo* DialogDescriptor, bool bIsPreview = false,
								  UObject* DialogDataSource = nullptr);
	TOptional<FString> GetInputWidgetString();
	void SetInputWidgetString(FString Input);
	
	void SetHeaderText(FText text);
	void SetContentText(FText text);

	UFUNCTION(Category = "Widgets")
	void SetDialogFocus();
};
