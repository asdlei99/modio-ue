/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/ModioEditableTextBox.h"

#include "ModioUI4Subsystem.h"
#include "Libraries/ModioSDKLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Core/Input/ModioInputKeys.h"
#include "UI/Styles/ModioEditableTextBoxStyle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScaleBox.h"

UModioEditableTextBox::UModioEditableTextBox(const FObjectInitializer& Initializer) : UEditableTextBox(Initializer)
{
	MinimumDesiredWidth = 200;
}

void UModioEditableTextBox::StartInput() 
{
	if (MyEditableTextBlock.IsValid())
	{
		FWidgetPath WidgetToFocusPath;
		
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(MyEditableTextBlock.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);

		if (!WidgetToFocusPath.IsValid()) 
		{
			return;
		}
		WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(MyEditableTextBlock);
	}
}

void UModioEditableTextBox::SynchronizeProperties()
{
	UEditableTextBox::SynchronizeProperties();
	const FModioEditableTextBoxStyle* ResolvedStyle = TextBoxStyle.FindStyle<FModioEditableTextBoxStyle>();
	if (ResolvedStyle)
	{
		MyEditableTextBlock->SetStyle(ResolvedStyle);
	}
}

FString UModioEditableTextBox::NativeGatherInput()
{
	return MyEditableTextBlock.IsValid() ? MyEditableTextBlock->GetText().ToString() : FString {};
}

TSharedRef<SWidget> UModioEditableTextBox::RebuildWidget()
{
	UEditableTextBox::RebuildWidget();

	const FModioEditableTextBoxStyle* ResolvedStyle = TextBoxStyle.FindStyle<FModioEditableTextBoxStyle>();
	if (ResolvedStyle)
	{
		MyEditableTextBlock->SetStyle(ResolvedStyle);
	}
	MyEditableTextBlock->SetClearKeyboardFocusOnCommit(true);
	// clang-format off
	return SAssignNew(MyVerticalBox, SVerticalBox)
	+SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBorder)
		.BorderImage_UObject(this, &UModioEditableTextBox::GetBorderImage)
		[
			SNew(SGridPanel).FillColumn(1, 1) 
			+SGridPanel::Slot(0, 0)
			[
				SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::Both)
				[
					SNew(SImage)
					.Image_UObject(this, &UModioEditableTextBox::GetHintGlyph)
					.Visibility_UObject(this, &UModioEditableTextBox::GetHintGlyphVisibility)
				]
			] 
			+SGridPanel::Slot(1, 0)
			[
				SAssignNew(MyEditableTextBlock, SEditableTextBox)
				.OnKeyDownHandler_UObject(this, &UModioEditableTextBox::OnKeyDownHandler)
				.OnTextChanged_UObject(this, &UModioEditableTextBox::ClampMaxCharacters)
			]
		]
	];
	// clang-format on
}

void UModioEditableTextBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyErrorTextBlock.Reset();
	MyVerticalBox.Reset();
}

void UModioEditableTextBox::NativeSetHintText(FText InHintText)
{
	if (MyEditableTextBlock)
	{
		MyEditableTextBlock->SetHintText(InHintText);
	}
}

const FSlateBrush* UModioEditableTextBox::GetHintGlyph() const
{
	const FModioEditableTextBoxStyle* ResolvedStyle = TextBoxStyle.FindStyle<FModioEditableTextBoxStyle>();
	if (ResolvedStyle)
	{
		return &ResolvedStyle->HintIcon;
	}
	return nullptr;
}

bool UModioEditableTextBox::HasValidationError() const
{
	return MyErrorTextBlock.IsValid() && !MyErrorTextBlock->GetText().IsEmpty();
}

const FSlateBrush* UModioEditableTextBox::GetBorderImage() const
{
	const FModioEditableTextBoxStyle* ResolvedStyle = TextBoxStyle.FindStyle<FModioEditableTextBoxStyle>();
	if (ResolvedStyle)
	{
		if (HasValidationError())
		{
			return &ResolvedStyle->ErrorBorderBrush;
		}
		if (IsHovered() || HasAnyUserFocus() || HasFocusedDescendants())
		{
			return &ResolvedStyle->HoveredBorderBrush;
		}
		return &ResolvedStyle->NormalBorderBrush;
	}
	return nullptr;
}

EVisibility UModioEditableTextBox::GetHintGlyphVisibility() const
{
	const FModioEditableTextBoxStyle* ResolvedStyle = TextBoxStyle.FindStyle<FModioEditableTextBoxStyle>();
	if (ResolvedStyle)
	{
		return ResolvedStyle->bShowHintIcon ? EVisibility::Visible : EVisibility::Collapsed;
	}
	return EVisibility::Collapsed;
}

void UModioEditableTextBox::NativeSetValidationError(FText ErrorText)
{
	const FModioRichTextStyle* ResolvedErrorTextStyle = TextStyle.FindStyle<FModioRichTextStyle>();
	const FTextBlockStyle* ErrorTextBlockStyle = &FTextBlockStyle::GetDefault();

	if (ResolvedErrorTextStyle)
	{
		ErrorStyleSet = ResolvedErrorTextStyle->CloneStyleSet();
		if (ErrorStyleSet)
		{
			ErrorTextBlockStyle = &ErrorStyleSet->GetWidgetStyle<FTextBlockStyle>(FName("Error"));
		}
	}

	if (ErrorText.IsEmpty())
	{
		if (MyErrorTextBlock.IsValid())
		{
			MyErrorTextBlock->SetText(ErrorText);
		}
		return;
	}

	if (!MyErrorTextBlock.IsValid())
	{
		MyVerticalBox->AddSlot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.Padding(0, 8, 0, 0)[SAssignNew(MyErrorTextBlock, SModioRichTextBlock)
									 .Text(ErrorText)
									 .WrapTextAt(700)
									 .TextStyle(ErrorTextBlockStyle)];
	}
	else
	{
		MyErrorTextBlock->SetText(ErrorText);
	}
}

FReply UModioEditableTextBox::OnKeyDownHandler(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		OnSubmit.Broadcast();
	}

	if (GetCommandKeyForEvent(InKeyEvent) == FModioInputKeys::Down)
	{
		OnNavigateDown.Broadcast();
	}

	UModioUI4Subsystem* subsystem = GEngine->GetEngineSubsystem<UModioUI4Subsystem>();

	if ((GetCommandKeyForEvent(InKeyEvent) == FModioInputKeys::Next || GetCommandKeyForEvent(InKeyEvent) == FModioInputKeys::Previous) && subsystem->GetLastInputDevice() != EModioUIInputMode::Keyboard)
	{
		return FReply::Unhandled();
	}
	return FReply::Unhandled();
}

void UModioEditableTextBox::ClampMaxCharacters(const FText& InText)
{
	if (MaxCharacters > 0)
	{
		if (GetText().ToString().Len() > MaxCharacters)
		{
			SetText(FText::FromString(GetText().ToString().Left(MaxCharacters)));
		}
	}
}

TSharedPtr<SEditableTextBox> UModioEditableTextBox::GetMyEditableTextBlock()
{
	return MyEditableTextBlock;
}

void UModioEditableTextBox::HandleOnTextChanged(const FText& InText)
{
	Super::HandleOnTextChanged(InText);

	if (bValidateInput)
	{
		FText ValidationError;
		NativeValidateText(MyEditableTextBlock->GetText(), ValidationError);
	}
}
