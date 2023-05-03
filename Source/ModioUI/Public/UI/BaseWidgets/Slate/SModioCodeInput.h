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

#include "Brushes/SlateNoResource.h"
#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "UI/Styles/ModioCodeInputStyle.h"
#include "UI/Interfaces/IModioInputMappingAccessor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_MULTICAST_DELEGATE(FOnNavigating);
DECLARE_MULTICAST_DELEGATE(FOnSubmit);

class SModioCodeInput : public SCompoundWidget, public IModioInputMappingAccessor
{
protected:
	TArray<TSharedPtr<class STextBlock>> CharacterWidgets;
	TSharedPtr<class SUniformGridPanel> MyCharacterGrid;
	TSharedPtr<class SEditableText> HiddenInputField;
	virtual TSharedRef<SWidget> CreateCharacterWidget(int32 WidgetIndex);
	FReply OnKeyDownHandler(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);
	const FModioCodeInputStyle* Style;
	FString CurrentText;
	virtual void RebuildChildren(uint32 NumChildren);
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	void OnInputTextChanged(const FText& NewText);
	FText GetCharacterAtIndex(int32 Index) const;
	FSlateNoResource NullBrush;
	FSlateBrush FakeCaretBrush;
	virtual bool SupportsKeyboardFocus() const override
	{
		return true;
	};
	virtual const FSlateBrush* GetCharacterBorder() const;
	virtual EVisibility GetCaretVisibility(uint32 Index) const;
	// Grrr getters always const
	mutable FSlateBrush CachedCharacterBrush;

public:
	SLATE_BEGIN_ARGS(SModioCodeInput) : _Style(&FModioCodeInputStyle::GetDefault()), _NumChildren(5) {}
	SLATE_ARGUMENT(const FModioCodeInputStyle*, Style);
	SLATE_ARGUMENT(int32, NumChildren);
	SLATE_END_ARGS();

	FOnNavigating OnNavigateDown;
	FOnSubmit OnSubmit;

	void Construct(const FArguments& InArgs);

	void SetStyle(const FModioCodeInputStyle* NewStyle);
	void SetNumChildren(int32 NewNumChildren);
	FString GetCodeString();
};