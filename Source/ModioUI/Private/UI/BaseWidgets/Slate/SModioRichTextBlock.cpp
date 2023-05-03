/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/Slate/SModioRichTextBlock.h"
#include "UI/Styles/ModioRichTextStyle.h"

void SModioRichTextBlock::Construct(const FArguments& InArgs)
{
	const ISlateStyle* StyleSet = InArgs._DecoratorStyleSet;
	const FTextBlockStyle* DefaultStyle = InArgs._TextStyle;

	StyleReference = InArgs._StyleReference;
	if (const FModioUIStyleRef* Style = StyleReference.Get())
	{
		const FModioRichTextStyle* ResolvedStyle = Style->FindStyle<FModioRichTextStyle>();
		if (ResolvedStyle)
		{
			StyleSetOverride = ResolvedStyle->CloneStyleSet();
			if (StyleSetOverride)
			{
				StyleSet = StyleSetOverride.Get();
				if (!DefaultStyle)
				{
					if (StyleSetOverride->HasWidgetStyle<FTextBlockStyle>(FName("Default")))
					{
						DefaultStyle = &StyleSetOverride->GetWidgetStyle<FTextBlockStyle>(FName("Default"));
					}
				}
			}
		}
	}
	if (!StyleSet)
	{
		UE_DEBUG_BREAK();
	}
	if (!DefaultStyle)
	{
		DefaultStyle = &FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	}
	SRichTextBlock::Construct(SRichTextBlock::FArguments()
								  .Text(InArgs._Text)
								  .HighlightText(InArgs._HighlightText)
								  .WrapTextAt(InArgs._WrapTextAt)
								  .AutoWrapText(InArgs._AutoWrapText)
								  .WrappingPolicy(InArgs._WrappingPolicy)
								  .Marshaller(InArgs._Marshaller)
								  .DecoratorStyleSet(StyleSet)
								  .TextStyle(DefaultStyle)
								  .Margin(InArgs._Margin)
								  .LineHeightPercentage(InArgs._LineHeightPercentage)
								  .Justification(InArgs._Justification)
								  .TextShapingMethod(InArgs._TextShapingMethod)
								  .TextFlowDirection(InArgs._TextFlowDirection)
								  .Decorators(InArgs._Decorators)
								  .Parser(InArgs._Parser)
								  .MinDesiredWidth(InArgs._MinDesiredWidth));

	ItemHoverSound = InArgs._HoveredSound;

	//SetOnMouseEnter(FNoReplyPointerEventHandler::CreateSP(this, &SModioRichTextBlock::PlayHoveredSound));
}

void SModioRichTextBlock::ReapplyStyle()
{
	if (const FModioUIStyleRef* Style = StyleReference.Get())
	{
		const FModioRichTextStyle* ResolvedStyle = Style->FindStyle<FModioRichTextStyle>();
		if (ResolvedStyle)
		{
			StyleSetOverride = ResolvedStyle->CloneStyleSet();
			if (StyleSetOverride)
			{
				SRichTextBlock::SetDecoratorStyleSet(StyleSetOverride.Get());

				if (StyleSetOverride->HasWidgetStyle<FTextBlockStyle>(FName("Default")))
				{
					SetTextStyle(StyleSetOverride->GetWidgetStyle<FTextBlockStyle>(FName("Default")));
				}
			}
		}
	}
}

bool SModioRichTextBlock::CustomPrepass(float InScale)
{
	ReapplyStyle();
	return SRichTextBlock::CustomPrepass(InScale);
}
