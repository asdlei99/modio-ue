/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/ModioCodeInputWidget.h"
#include "UI/BaseWidgets/Slate/SModioCodeInput.h"
#include "UI/Styles/ModioCodeInputStyle.h"
#include "Widgets/Layout/SUniformGridPanel.h"

FEventReply UModioCodeInputWidget::NativeValidateCodeInputCharacter(FString Character)
{
	return FEventReply(true);
}

FString UModioCodeInputWidget::NativeGatherInput()
{
	if (MyInput)
	{
		return MyInput->GetCodeString();
	}
	return {};
}

FEventReply UModioCodeInputWidget::ValidateCodeInputCharacter_Implementation(const FString& Character)
{
	return NativeValidateCodeInputCharacter(Character);
}

void UModioCodeInputWidget::SynchronizeProperties()
{
	UWidget::SynchronizeProperties();
}

TSharedRef<SWidget> UModioCodeInputWidget::RebuildWidget()
{
	const FModioCodeInputStyle* ResolvedStyle = Style.FindStyle<FModioCodeInputStyle>();
	return SAssignNew(MyVerticalBox, SVerticalBox) +
		   SVerticalBox::Slot()
			   .AutoHeight()[SAssignNew(MyInput, SModioCodeInput)
								 .NumChildren(NumberOfCharacters)
								 .Style(ResolvedStyle ? ResolvedStyle : &FModioCodeInputStyle::GetDefault())];
}

void UModioCodeInputWidget::NativeSetValidationError(FText ErrorText)
{
	const FModioRichTextStyle* ResolvedErrorTextStyle = TextStyle.FindStyle<FModioRichTextStyle>();
	const FTextBlockStyle* ErrorTextBlockStyle = &FTextBlockStyle::GetDefault();

	if (ResolvedErrorTextStyle)
	{
		TSharedPtr<FSlateStyleSet> StyleSet = ResolvedErrorTextStyle->CloneStyleSet();
		if (StyleSet)
		{
			ErrorTextBlockStyle = &StyleSet->GetWidgetStyle<FTextBlockStyle>(FName("Error"));
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
									 .StyleReference(&TextStyle)
									 .TextStyle(ErrorTextBlockStyle)];
	}
	else
	{
		MyErrorTextBlock->SetText(ErrorText);
	}
}

TSharedPtr<SModioCodeInput> UModioCodeInputWidget::GetMyInput()
{
	return MyInput;
}
