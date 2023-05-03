/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/Dialogs/ModioDialogBaseInternal.h"

#include "Blueprint/UserWidget.h"
#include "Core/ModioModInfoUI.h"
#include "Core/ModioReportInfoUI.h"
#include "Core/ModioUIHelpers.h"
#include "Misc/EngineVersionComparison.h"
#include "Styling/SlateStyle.h"
#include "UI/BaseWidgets/ModioLoadingSpinner.h"
#include "UI/BaseWidgets/ModioUIAsyncLoadingOverlay.h"
#include "UI/BaseWidgets/Slate/SModioRichTextBlock.h"
#include "UI/BaseWidgets/Slate/SModioButtonBase.h"
#include "UI/BaseWidgets/Slate/SModioTileView.h"
#include "UI/BaseWidgets/ModioReportButtonGroupWidget.h"
#include "UI/BaseWidgets/ModioEditableTextBox.h"
#include "UI/BaseWidgets/ModioCodeInputWidget.h"
#include "UI/BaseWidgets/Slate/SModioCodeInput.h"
#include "UI/Commands/ModioCommonUICommands.h"
#include "UI/CommonComponents/ModioAuthenticationMethodSelector.h"
#include "UI/Dialogs/ModioDialogController.h"
#include "UI/Dialogs/ModioDialogInfo.h"
#include "UI/Interfaces/IModioAuthenticationContextUIDetails.h"
#include "UI/Interfaces/IModioUIDataSourceWidget.h"
#include "UI/Interfaces/IModioUIDialogButtonWidget.h"
#include "UI/Interfaces/IModioUIStringInputWidget.h"
#include "UI/Interfaces/IModioUITextValidator.h"
#include "UI/Styles/ModioButtonStyle.h"
#include "UI/Styles/ModioDialogStyle.h"
#include "UI/Styles/ModioRichTextStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STileView.h"

void UModioDialogBaseInternal::BuildCommandList(TSharedRef<FUICommandList> InCommandList)
{
	// Explicitly eat these commands so that they don't propagate outside the dialog
	// These could probably be set on the dialog host instead?
	InCommandList->MapAction(FModioCommonUICommands::Get().MoreOptions, FModioCommonUICommands::GetNullAction());
	InCommandList->MapAction(FModioCommonUICommands::Get().RefineSearch, FModioCommonUICommands::GetNullAction());
	InCommandList->MapAction(FModioCommonUICommands::Get().NextPage, FModioCommonUICommands::GetNullAction());
	InCommandList->MapAction(FModioCommonUICommands::Get().PreviousPage, FModioCommonUICommands::GetNullAction());
	InCommandList->MapAction(FModioCommonUICommands::Get().DownloadQueue, FModioCommonUICommands::GetNullAction());
}

TSharedRef<SWidget> UModioDialogBaseInternal::RebuildWidget()
{
	ButtonInfos.Empty();

	FMargin BackgroundPadding;
	if (Controller.IsValid() && Controller->LoadingOverlay)
	{
		if (!LoadingSpinner)
		{
			LoadingSpinner = NewObject<UWidget>(this, Controller->LoadingOverlay);
			Cast<UModioUIAsyncLoadingOverlay>(LoadingSpinner)->SetOwningDialog(this);
		}
	}

	// clang-format off
	SAssignNew(DialogInternalRoot, SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(BackgroundImage, SImage)
		]
		+SOverlay::Slot()
		.Expose(DialogContentOverlaySlot)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(BackgroundPadding)
		[
			SNew(SBox)
			.Padding(FMargin(0.0f))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.MaxDesiredWidth(880.0f)
			//.MinDesiredWidth(880.0f)
			[
				SAssignNew(ContentPanel, SGridPanel)
				.FillRow(1, 1)
				+ SGridPanel::Slot(0, 0)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(0,0,0,24)
				[
					SAssignNew(HeadingTextBlock, SModioRichTextBlock)
					.WrapTextAt(800)
				]
				
				+SGridPanel::Slot(0,1)
				.Expose(SubHeaderWidgetSlot)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(0,0,0,24)
				[
					SNullWidget::NullWidget
				]

				+ SGridPanel::Slot(0, 2)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Fill)
				.Expose(DialogTextBlockSlot)
				.Padding(FMargin(24,24,24,24))
				[
					SAssignNew(DialogTextBlock,SModioRichTextBlock)
					.WrapTextAt(800)
				]
				+ SGridPanel::Slot(0, 3)
				.Expose(InputWidgetSlot)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(24,24,24,24)
				[
					SNullWidget::NullWidget
				]
				+ SGridPanel::Slot(0, 4)
				.HAlign(HAlign_Center)
				.Padding(0, 24, 0, 0)
				[
					SAssignNew(DialogButtons, SModioTileView<TSharedPtr<FModioDialogButtonInfo>>)
					.Orientation(Orient_Horizontal)
					.ListItemsSource(&ButtonParams)
					.ItemHeight(0)
					.ItemWidth(0)
					.IsFocusable(false)						// There should be no reason to be able to focus on a slot element
					.SelectionMode(ESelectionMode::None)
					.WrapHorizontalNavigation(false)
					.HandleGamepadEvents(false)
					.HandleDirectionalNavigation(false)
					.AllowOverscroll(EAllowOverscroll::No)
					.Visibility_UObject(this, &UModioDialogBaseInternal::GetButtonListVisibility)
					.OnGenerateTile_UObject(this, &UModioDialogBaseInternal::OnGenerateButton)
				]
				+SGridPanel::Slot(0,5)
				.Expose(ButtonWidgetSlot)
				.HAlign(HAlign_Fill)
				[
					SNullWidget::NullWidget
				]
			]
		]
		+SOverlay::Slot()
		.Expose(LoadingSpinnerOverlaySlot)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNullWidget::NullWidget
		];

	if (!bUsingCustomButtons) {
		auto Button = DialogButtons->GetChildren()->GetChildAt(0);
		FSlateApplication::Get().SetUserFocus(0, Button, EFocusCause::SetDirectly);
	}

	return DialogInternalRoot.ToSharedRef();
	// clang-format on
}

void UModioDialogBaseInternal::ShowLoadingSpinner()
{
	if (LoadingSpinner)
	{
		if (ContentPanel.IsValid())
		{
			ContentPanel->SetVisibility(EVisibility::Collapsed);
		}
		LoadingSpinnerOverlaySlot->AttachWidget(LoadingSpinner->TakeWidget());

		if (UModioUIAsyncLoadingOverlay* spinner = Cast<UModioUIAsyncLoadingOverlay>(LoadingSpinner))
		{
			spinner->SetDialogFocus();
		}
	}
}

void UModioDialogBaseInternal::HideLoadingSpinner()
{
	if (LoadingSpinner)
	{
		if (ContentPanel.IsValid())
		{
			ContentPanel->SetVisibility(EVisibility::Visible);
		}
		LoadingSpinnerOverlaySlot->DetachWidget();
	}
}

void UModioDialogBaseInternal::SynchronizeProperties()
{
	UWidget::SynchronizeProperties();
	ApplyStyling();
}

void UModioDialogBaseInternal::ApplyStyling()
{
	const FModioDialogStyle* ResolvedStyle = DialogStyle.FindStyle<FModioDialogStyle>();
	if (ResolvedStyle)
	{
		if (HeadingTextBlock)
		{
			const FModioRichTextStyle* ResolvedTitleTextStyle =
				ResolvedStyle->TitleTextStyle.FindStyle<FModioRichTextStyle>();
			if (ResolvedTitleTextStyle)
			{
				HeadingTextBlock->StyleReference = &ResolvedStyle->TitleTextStyle;
				HeadingTextBlock->ReapplyStyle();
			}
		}
		if (DialogTextBlock)
		{
			const FModioRichTextStyle* ResolvedContentTextStyle =
				ResolvedStyle->ContentTextStyle.FindStyle<FModioRichTextStyle>();
			if (ResolvedContentTextStyle)
			{
				DialogTextBlock->StyleReference = &ResolvedStyle->ContentTextStyle;
				DialogTextBlock->ReapplyStyle();
			}
		}
		if (BackgroundImage)
		{
			BackgroundImage->SetImage(&ResolvedStyle->BackgroundBrush);
		}
		if (DialogContentOverlaySlot)
		{
			ModioUIHelpers::SetPadding(*DialogContentOverlaySlot, ResolvedStyle->ContentPadding);
		}
	}
}

void UModioDialogBaseInternal::UpdateBoundValues(class UModioDialogInfo* DialogDescriptor)
{
	if (DialogDescriptor)
	{
		ButtonParams.Empty();
		for (const FModioDialogButtonInfo& ButtonInfo : DialogDescriptor->Buttons)
		{
			ButtonParams.Add(MakeShared<struct FModioDialogButtonInfo>(ButtonInfo));
		}
		DialogButtons->RequestListRefresh();
	}
}

EVisibility UModioDialogBaseInternal::GetButtonListVisibility() const
{
	return bUsingCustomButtons ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply UModioDialogBaseInternal::OnButtonClicked(TSharedPtr<FModioDialogButtonInfo> Button)
{
	if (!bBeingPreviewed)
	{
		if (!Controller.IsValid())
		{
			// For some reason we dont have a dialog controller, so early-out
			return FReply::Handled();
		}
		// process the button click here
		switch (Button->ButtonCommand)
		{
			case EModioDialogButtonCommand::Back:
				Controller->PopDialog();
				break;
			case EModioDialogButtonCommand::PushDialog:
				if (IsInputValid())
				{
					Controller->PushDialog(Button->Destination, DataSource);
				}

				break;
			case EModioDialogButtonCommand::Close:
				Controller->FinalizeDialog();
				break;
			case EModioDialogButtonCommand::Cancel:
				Controller->FinalizeDialog();
				break;
			case EModioDialogButtonCommand::AsyncCallThenPushDialog:
			{
				switch (Button->AsyncCallType)
				{
					case EModioDialogAsyncCall::AuthSubmitEmail:
						if (TOptional<FString> Input = GetInputWidgetString())
						{
							if (IsInputValid())
							{
								Controller->ShowLoadingDialog();
								Controller->SendEmailCodeRequest(Input.GetValue(), Button->Destination);
							}
						}
						break;
					case EModioDialogAsyncCall::AuthSubmitCode:
						if (TOptional<FString> Input = GetInputWidgetString())
						{
							if (IsInputValid())
							{
								Controller->ShowLoadingDialog();
								Controller->SubmitEmailAuthCode(Input.GetValue());
							}
						}
						break;
					case EModioDialogAsyncCall::BeginAuthentication:
						// Called on accepting TermsOfUseDialog
						if (DataSource)
						{
							if (DataSource->Implements<UModioAuthenticationContextUIDetails>())
							{
								// ProviderInfo is set by user selection on ModioAuthenticationMethodDialog
								// Selected Destination in editor is ignored
								FModioUIAuthenticationProviderInfo ProviderInfo =
									IModioAuthenticationContextUIDetails::Execute_GetProviderInfo(DataSource);
								if (ProviderInfo.bIsEmailAuthentication)
								{
									Controller->ShowEmailAuthenticationDialog();
								}
								else if (UModioUISubsystem* Subsystem =
											 GEngine->GetEngineSubsystem<UModioUISubsystem>())
								{
									Subsystem->RequestExternalAuthentication(ProviderInfo.ProviderID);
								}
							}
						}
						break;
					case EModioDialogAsyncCall::RetryFetchTerms:
						// Note that RetryFetchTerms is specific to the Retry button on TermsOfUseFailDialog.  Recheck
						// functionality if using for a general FetchTerms case in the future.
						// Consider refactoring this into a specialized set of buttons?
						if (DataSource)
						{
							if (DataSource->Implements<UModioAuthenticationContextUIDetails>())
							{
								FModioUIAuthenticationProviderInfo ProviderInfo =
									IModioAuthenticationContextUIDetails::Execute_GetProviderInfo(DataSource);

								Controller->ShowLoadingDialog();
								// Selected Destination in editor is ignored
								Controller->ShowTermsOfUseDialog(
									MakeShared<FModioUIAuthenticationProviderInfo>(ProviderInfo));
							}
						}
						break;
					case EModioDialogAsyncCall::UnsubscribeFromMod:
					{
						if (DataSource)
						{
							if (DataSource->Implements<UModioModInfoUIDetails>())
							{
								FModioModInfo ModInfo = IModioModInfoUIDetails::Execute_GetFullModInfo(DataSource);

								Controller->RequestUnsubscribe(ModInfo.ModId, Button->Destination);
							}
						}
					}
					break;
					case EModioDialogAsyncCall::UninstallMod:
						if (DataSource)
						{
							if (DataSource->Implements<UModioModInfoUIDetails>())
							{
								FModioModInfo ModInfo = IModioModInfoUIDetails::Execute_GetFullModInfo(DataSource);

								Controller->RequestUninstall(ModInfo.ModId, Button->Destination);
							}
						}
						break;
					case EModioDialogAsyncCall::Logout:
						Controller->LogOut();
						break;

					case EModioDialogAsyncCall::SubmitReport:
						UModioReportInfoUI* ReportInfo = Cast<UModioReportInfoUI>(DataSource);
						if (ReportInfo)
						{
							Controller->ReportContentAsync(ReportInfo->ReportData, Button->Destination);
						}
						break;
				}
				break;
			}
			case EModioDialogButtonCommand::PushDialogWithOperation:
			{
				switch (Button->OperationCallType)
				{
					case EModioDialogOperationCall::SetReportEmailAddress:
						if (GetInputWidgetString().IsSet())
						{
							Cast<UModioReportInfoUI>(DataSource)->ReportData.ReporterContact =
								GetInputWidgetString().GetValue();
						}

						if (IsInputValid())
						{
							Controller->PushDialog(Button->Destination);
						}

						break;
					case EModioDialogOperationCall::SetReportDetails:
						if (GetInputWidgetString().IsSet())
						{
							Cast<UModioReportInfoUI>(DataSource)->ReportData.ReportDescription =
								GetInputWidgetString().GetValue();
						}

						if (IsInputValid())
						{
							Controller->PushDialog(Button->Destination);
						}

						break;
					default:;
				}
			}
			break;
		}
	}
	return FReply::Handled();
}

TSharedRef<class ITableRow> UModioDialogBaseInternal::OnGenerateButton(
	TSharedPtr<struct FModioDialogButtonInfo> ButtonInfo, const TSharedRef<class STableViewBase>& OwnerTableView)
{
	ButtonInfos.Add(ButtonInfo);

	TSharedPtr<SButton> RowButton;
	TSharedPtr<SModioRichTextBlock> RowTextBlock;
#if UE_VERSION_NEWER_THAN(5, 0, 0)
	// UE5 changes the default FTableRowStyle to have a black background, grr
	static FTableRowStyle StyleOverride = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	StyleOverride.SetEvenRowBackgroundBrush(FSlateColorBrush(FSlateColor(FLinearColor::White)))
		.SetOddRowBackgroundBrush(FSlateColorBrush(FSlateColor(FLinearColor::White)))
		.SetEvenRowBackgroundHoveredBrush(FSlateColorBrush(FSlateColor(FLinearColor::White)))
		.SetOddRowBackgroundHoveredBrush(FSlateColorBrush(FSlateColor(FLinearColor::White)));
#endif
	// clang-format off
	TSharedRef<STableRow<TSharedPtr<FText>>> TableRow = SNew(STableRow<TSharedPtr<FText>>, OwnerTableView)
	#if UE_VERSION_NEWER_THAN(5,0,0) 
		// Hacky, but Epic don't let you set a style on an STableRow post-construction, extra grr
		.Style(&StyleOverride)
	#endif 
		.Content()
		[
			SAssignNew(RowButton, SModioButtonBase)
			.OnClicked_UObject(this, &UModioDialogBaseInternal::OnButtonClicked, ButtonInfo)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.IsFocusable(true)
			.ContentPadding(FMargin(16, 4,16, 2))
			[
				SAssignNew(RowTextBlock, SModioRichTextBlock).Text(ButtonInfo->ButtonLabel)
				.StyleReference_UObject(this, &UModioDialogBaseInternal::GetButtonTextStyle)
			]
		];
	// clang-format on
	GeneratedButtons.Add(RowButton);
	SetDialogFocus();
	const FModioDialogStyle* ResolvedDialogStyle = DialogStyle.FindStyle<FModioDialogStyle>();
	if (ResolvedDialogStyle)
	{
		const FModioButtonStyle* ResolvedButtonStyle = ResolvedDialogStyle->ButtonStyle.FindStyle<FModioButtonStyle>();
		if (ResolvedButtonStyle)
		{
			RowButton->SetButtonStyle(&ResolvedButtonStyle->ButtonStyle);
			RowButton->SetContentPadding(ResolvedButtonStyle->ContentPadding);
		}
	}
	TableRow->SetPadding(16.f);

	return TableRow;
}

const FModioUIStyleRef* UModioDialogBaseInternal::GetButtonTextStyle() const
{
	const FModioDialogStyle* ResolvedDialogStyle = DialogStyle.FindStyle<FModioDialogStyle>();
	if (ResolvedDialogStyle)
	{
		const FModioButtonStyle* ResolvedButtonStyle = ResolvedDialogStyle->ButtonStyle.FindStyle<FModioButtonStyle>();
		if (ResolvedButtonStyle)
		{
			return &ResolvedButtonStyle->RichTextStyle;
		}
	}
	return nullptr;
}

void UModioDialogBaseInternal::OnNavigateDownFromCodeInputWidget()
{
	if (bUsingCustomButtons)
	{
		IModioUIDialogButtonWidget* buttons = Cast<IModioUIDialogButtonWidget>(ButtonWidget);
		if (buttons && buttons->Buttons.IsValidIndex(0))
		{
			FSlateApplication::Get().SetKeyboardFocus(buttons->Buttons[0]);
			return;
		}
	}
	else if (GeneratedButtons.IsValidIndex(0))
	{
		FSlateApplication::Get().SetKeyboardFocus(GeneratedButtons[0]);
	}
}

void UModioDialogBaseInternal::OnSubmitKeyPressed()
{
	TOptional<FString> Input = GetInputWidgetString();
	if (!Input || !IsInputValid())
	{
		return;
	}

	UModioEditableTextBox* inputWidget = Cast<UModioEditableTextBox>(InputWidget);
	if (inputWidget)
	{
		for (auto& button : ButtonInfos)
		{
			if (button->AsyncCallType == EModioDialogAsyncCall::AuthSubmitEmail && IsValid(button->Destination))
			{
				Controller->ShowLoadingDialog();
				Controller->SendEmailCodeRequest(Input.GetValue(), button->Destination);
				return;
			}
		}
	}

	UModioCodeInputWidget* codeInput = Cast<UModioCodeInputWidget>(InputWidget);
	if (codeInput)
	{
		TSharedPtr<SModioCodeInput> EditableTextBox = codeInput->GetMyInput();
		if (EditableTextBox)
		{
			Controller->ShowLoadingDialog();
			Controller->SubmitEmailAuthCode(Input.GetValue());
		}
	}
}

void UModioDialogBaseInternal::SetDialogController(UModioDialogController* DialogController)
{
	Controller = DialogController;
}

void UModioDialogBaseInternal::InitializeFromDialogInfo(class UModioDialogInfo* DialogDescriptor, bool bIsPreview,
														UObject* DialogDataSource)
{
	bBeingPreviewed = bIsPreview;
	GeneratedButtons.Empty();
	InputWidget = nullptr;
	if (DialogDescriptor)
	{
		if (DialogDataSource != nullptr)
		{
			DataSource = DialogDataSource;
		}
		if (HeadingTextBlock)
		{
			HeadingTextBlock->SetText(DialogDescriptor->TitleText);
		}

		if (DialogTextBlock)
		{
			if (DialogDescriptor->DialogText.IsEmptyOrWhitespace())
			{
				ModioUIHelpers::SetPadding(*DialogTextBlockSlot, 0);
				DialogTextBlock->SetText(FText::FromString(FString()));
			}
			else
			{
				DialogTextBlock->SetText(DialogDescriptor->DialogText);
			}
		}

		if (SubHeaderWidgetSlot)
		{
			ContentPanel->RemoveSlot(SubHeaderWidgetSlot->GetWidget());
		}
		SubHeaderWidgetSlot = ModioUIHelpers::AddSlot(ContentPanel.Get(), 0, 1);

		if (InputWidgetSlot)
		{
			ContentPanel->RemoveSlot(InputWidgetSlot->GetWidget());
		}
		InputWidgetSlot = ModioUIHelpers::AddSlot(ContentPanel.Get(), 0, 3);

		if (ButtonWidgetSlot)
		{
			ContentPanel->RemoveSlot(ButtonWidgetSlot->GetWidget());
		}
		ButtonWidgetSlot = ModioUIHelpers::AddSlot(ContentPanel.Get(), 0, 5);

		if (SubHeaderWidgetSlot)
		{
			if (DialogDescriptor->SubHeaderWidget)
			{
				UClass* SubHeaderWidgetClass = DialogDescriptor->SubHeaderWidget.Get();
				SubHeaderWidget = nullptr;
				if (SubHeaderWidgetClass->IsChildOf(UUserWidget::StaticClass()))
				{
					SubHeaderWidget = CreateWidget<UUserWidget>(this, SubHeaderWidgetClass);
				}
				else if (SubHeaderWidgetClass->IsChildOf(UWidget::StaticClass()))
				{
					SubHeaderWidget = NewObject<UWidget>(this, SubHeaderWidgetClass);
				}
				if (SubHeaderWidget)
				{
					if (SubHeaderWidget->Implements<UModioUIDataSourceWidget>())
					{
						IModioUIDataSourceWidget::Execute_SetDataSource(SubHeaderWidget, DataSource);
					}

					TSharedPtr<SBox> MySizeBox = nullptr;
					SubHeaderWidgetSlot->AttachWidget(
						SAssignNew(MySizeBox, SBox)[SNew(SScaleBox)
														.HAlign(DialogDescriptor->SubHeaderWidgetHAlign)
														.VAlign(DialogDescriptor->SubHeaderWidgetVAlign)
														.StretchDirection(EStretchDirection::Both)
														.Stretch(EStretch::ScaleToFit)[SubHeaderWidget->TakeWidget()]]);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
					SubHeaderWidgetSlot->NotifySlotChanged(false);
#endif
				}
			}
			else
			{
				SubHeaderWidgetSlot->AttachWidget(SNullWidget::NullWidget);
				ContentPanel->SetRowFill(2, 0);
			}
		}

		if (InputWidgetSlot)
		{
			if (DialogDescriptor->InputWidget)
			{
				UClass* InputWidgetClass = DialogDescriptor->InputWidget.Get();
				if (InputWidgetClass->IsChildOf(UUserWidget::StaticClass()))
				{
					InputWidget = CreateWidget<UUserWidget>(this, InputWidgetClass);
				}
				else if (InputWidgetClass->IsChildOf(UWidget::StaticClass()))
				{
					InputWidget = NewObject<UWidget>(this, InputWidgetClass);
				}
				if (InputWidget)
				{
					IModioUIStringInputWidget::Execute_SetHint(InputWidget, DialogDescriptor->InputWidgetHintText);
					TSharedPtr<SBox> MySizeBox = nullptr;

					if (InputWidget->Implements<UModioUIDataSourceWidget>())
					{
						IModioUIDataSourceWidget::Execute_SetDataSource(InputWidget, DataSource);
					}

					InputWidgetSlot->AttachWidget(
						SAssignNew(MySizeBox, SBox)[SNew(SScaleBox)
														.HAlign(DialogDescriptor->InputWidgetHAlign)
														.VAlign(DialogDescriptor->InputWidgetVAlign)
														.StretchDirection(DialogDescriptor->InputWidgetScalingType)
														.Stretch(EStretch::Fill)[InputWidget->TakeWidget()]]);
					if (!FMath::IsNearlyZero(DialogDescriptor->InputWidgetHeightOverride))
					{
						MySizeBox->SetHeightOverride(DialogDescriptor->InputWidgetHeightOverride);
					}
#if UE_VERSION_OLDER_THAN(5, 0, 0)

					InputWidgetSlot->NotifySlotChanged(false);
#endif
				}
				else
				{
					ModioUIHelpers::SetPadding(*InputWidgetSlot, 0);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
					InputWidgetSlot->NotifySlotChanged(false);
#endif
				}
			}
			else
			{
				InputWidgetSlot->AttachWidget(SNullWidget::NullWidget);
				ContentPanel->SetRowFill(2, 0);

				ModioUIHelpers::SetPadding(*InputWidgetSlot, 0);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
				InputWidgetSlot->NotifySlotChanged(false);
#endif
			}
		}

		if (ButtonWidgetSlot)
		{
			if (DialogDescriptor->ButtonAreaWidget)
			{
				UClass* ButtonWidgetClass = DialogDescriptor->ButtonAreaWidget.Get();
				ButtonWidget = nullptr;

				if (ButtonWidgetClass->IsChildOf(UUserWidget::StaticClass()))
				{
					ButtonWidget = CreateWidget<UUserWidget>(this, ButtonWidgetClass);
				}
				else if (ButtonWidgetClass->IsChildOf(UWidget::StaticClass()))
				{
					ButtonWidget = NewObject<UWidget>(this, ButtonWidgetClass);
				}
				if (ButtonWidget)
				{
					if (ButtonWidget->Implements<UModioUIDataSourceWidget>())
					{
						IModioUIDataSourceWidget::Execute_SetDataSource(ButtonWidget, DataSource);
					}

					// Calling TakeWidget before setting the style so that we're guaranteed to have underlying SWidgets
					ModioUIHelpers::SetHorizontalAlignment(*ButtonWidgetSlot, DialogDescriptor->ButtonAreaWidgetHAlign);
					ModioUIHelpers::SetVerticalAlignment(*ButtonWidgetSlot, DialogDescriptor->ButtonAreaWidgetVAlign);

					ButtonWidgetSlot->AttachWidget(ButtonWidget->TakeWidget());
#if UE_VERSION_OLDER_THAN(5, 0, 0)
					ButtonWidgetSlot->NotifySlotChanged(false);
#endif

					IModioUIDialogButtonWidget::Execute_SetDialogController(ButtonWidget, Controller.Get());

					if (const FModioDialogStyle* ResolvedDialogStyle = DialogStyle.FindStyle<FModioDialogStyle>())
					{
						IModioUIDialogButtonWidget::Execute_SetStyle(ButtonWidget, *ResolvedDialogStyle);
					}

					bUsingCustomButtons = true;
				}
				else
				{
					bUsingCustomButtons = false;
					ModioUIHelpers::SetPadding(*InputWidgetSlot, 0);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
					InputWidgetSlot->NotifySlotChanged(false);
#endif
				}
			}
			else
			{
				bUsingCustomButtons = false;
				UpdateBoundValues(DialogDescriptor);
				ButtonWidgetSlot->AttachWidget(SNullWidget::NullWidget);
				ContentPanel->SetRowFill(4, 0);
				ModioUIHelpers::SetPadding(*InputWidgetSlot, 0);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
				InputWidgetSlot->NotifySlotChanged(false);
#endif
			}
		}
	}

	if (InputWidget) 
	{
		FCustomWidgetNavigationDelegate navDelegate;
		navDelegate.BindUFunction(this, "OnNavigateDownFromCodeInputWidget");
		InputWidget->SetNavigationRuleCustom(EUINavigation::Down, navDelegate);
	}	

	UModioEditableTextBox* inputWidget = Cast<UModioEditableTextBox>(InputWidget);
	if (inputWidget)
	{
		inputWidget->OnSubmit.AddUFunction(this, FName("OnSubmitKeyPressed"));
		inputWidget->OnNavigateDown.AddUFunction(this, FName("OnNavigateDownFromCodeInputWidget"));
	}

	UModioCodeInputWidget* codeInput = Cast<UModioCodeInputWidget>(InputWidget);
	if (codeInput)
	{
		TSharedPtr<SModioCodeInput> EditableTextBox = codeInput->GetMyInput();
		if (EditableTextBox)
		{
			EditableTextBox->OnSubmit.AddUFunction(this, FName("OnSubmitKeyPressed"));
			EditableTextBox->OnNavigateDown.AddUFunction(this, FName("OnNavigateDownFromCodeInputWidget"));
		}
	}


	ApplyStyling();
}

TOptional<FString> UModioDialogBaseInternal::GetInputWidgetString()
{
	if (InputWidget && InputWidget->GetClass()->ImplementsInterface(UModioUIStringInputWidget::StaticClass()))
	{
		return IModioUIStringInputWidget::Execute_GatherInput(InputWidget);
	}
	return {};
}

bool UModioDialogBaseInternal::IsInputValid()
{
	// Input is always valid if we don't have an input widget
	if (!InputWidget)
	{
		return true;
	}

	if (InputWidget->Implements<UModioUITextValidator>())
	{
		if (GetInputWidgetString().IsSet())
		{
			FText Reason;
			return IModioUITextValidator::Execute_ValidateText(
				InputWidget, FText::FromString(GetInputWidgetString().GetValue()), Reason);
		}
	}

	return true;
}

void UModioDialogBaseInternal::SetInputWidgetString(FString Input)
{
	if (InputWidget)
	{
		IModioUIStringInputWidget::Execute_SetInput(InputWidget, Input);
	}
}

void UModioDialogBaseInternal::SetHeaderText(FText text) 
{
	if (!HeadingTextBlock) 
	{
		return;	
	}

	HeadingTextBlock->SetText(text);
}

void UModioDialogBaseInternal::SetContentText(FText text)
{
	if (!DialogTextBlock)
	{
		return;
	}

	DialogTextBlock->SetText(text);
}

void UModioDialogBaseInternal::SetDialogFocus()
{
	UModioEditableTextBox* inputWidget = Cast<UModioEditableTextBox>(InputWidget);
	UModioCodeInputWidget* codeInput = Cast<UModioCodeInputWidget>(InputWidget);

	if (inputWidget)
	{
		TSharedPtr<SEditableTextBox> EditableTextBox = inputWidget->GetMyEditableTextBlock();
		FSlateApplication::Get().SetKeyboardFocus(EditableTextBox);		
		return;
	}

	else if (codeInput)
	{
		TSharedPtr<SModioCodeInput> EditableTextBox = codeInput->GetMyInput();
		FSlateApplication::Get().SetKeyboardFocus(EditableTextBox);
		return;
	}

	else
	{
		if (bUsingCustomButtons)
		{
			IModioUIDialogButtonWidget* buttons = Cast<IModioUIDialogButtonWidget>(ButtonWidget);
			if (buttons && buttons->Buttons.IsValidIndex(0))
			{
				FSlateApplication::Get().SetKeyboardFocus(buttons->Buttons[0]);
				return;
			}
		}
		else if (GeneratedButtons.IsValidIndex(0))
		{
			FSlateApplication::Get().SetKeyboardFocus(GeneratedButtons[0]);
		}
	}
}
