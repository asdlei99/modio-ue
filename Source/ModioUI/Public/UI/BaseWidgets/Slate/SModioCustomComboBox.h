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

#include "Brushes/SlateColorBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"
#include "Rendering/DrawElements.h"
#include "Slate/SRetainerWidget.h"
#include "Types/SlateEnums.h"
#include "UI/Styles/ModioUIColorRef.h"
#include "UI/Styles/ModioWidgetBorderStyle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

template<typename DataType>
class SModioCustomComboBox : public SCompoundWidget
{
protected:
	const FButtonStyle InvisibleButtonStyle = FButtonStyle()
												  .SetNormal(FSlateNoResource())
												  .SetHovered(FSlateNoResource())
												  .SetPressed(FSlateNoResource())
												  .SetNormalPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
												  .SetPressedPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f));

	TSharedRef<SWidget> CreateButtonContentWidget()
	{
		if (UnderlyingData && UnderlyingData->Num())
		{
			if (OnGetButtonContent.IsBound())
			{
				if (CurrentSelectionIndex)
				{ 
					return SNew(SButton)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.ButtonStyle(&InvisibleButtonStyle)
						.OnPressed(this, &SModioCustomComboBox::OnButtonPressed)
						.ContentPadding(ContentPadding)
						.Content()[OnGetButtonContent.Execute((*UnderlyingData)[CurrentSelectionIndex.GetValue()])];
				}
				else
				{
					return SNew(SButton)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.ButtonStyle(&InvisibleButtonStyle)
						.OnPressed(this, &SModioCustomComboBox::OnButtonPressed)
						.ContentPadding(ContentPadding)
						.Content()[OnGetButtonContent.Execute((*UnderlyingData)[0])];
				}
			}
		}
		// Perhaps return the button with empty content here instead?
		return SNew(SButton)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.ButtonStyle(&InvisibleButtonStyle)
			.OnPressed(this, &SModioCustomComboBox::OnButtonPressed)
			.ContentPadding(ContentPadding)
			.Content()[SNew(STextBlock).Text(FText::FromString("No data!"))];
	}

	TSharedPtr<SListView<DataType>> CreateListViewWidget()
	{

		return SNew(SListView<DataType>)
			.SelectionMode(ESelectionMode::Single)
			.OnGenerateRow(OnGenerateOptionWidget)
			.ListItemsSource(UnderlyingData)
			.HandleDirectionalNavigation(true)
			.HandleGamepadEvents(true)
			.OnSelectionChanged(this, &SModioCustomComboBox::OnSelectionChanged)
			.Visibility(this, &SModioCustomComboBox::GetMenuVisibility);
	}

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		bIsHovered = true;
		SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);

		FSlateApplication::Get().PlaySound(HoveredSound);
	}
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		bIsHovered = false;
		SCompoundWidget::OnMouseLeave(MouseEvent);
	}
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
								 const FFocusEvent& InFocusEvent)
	{
#if UE_VERSION_NEWER_THAN(5, 0, 0)
		const bool bDescendantNewlyFocused = NewWidgetPath.ContainsWidget(this);
#else
		const bool bDescendantNewlyFocused = NewWidgetPath.ContainsWidget(this->AsShared());
#endif
		if (!bDescendantNewlyFocused)
		{
			if (bIsOpen)
			{
				Close();
			}
			bIsHovered = false;
		}
		else
		{
			bIsHovered = true;
			FSlateApplication::Get().PlaySound(HoveredSound);
		}

		SCompoundWidget::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
	}
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override
	{
		SCompoundWidget::OnFocusLost(InFocusEvent);
		if (bIsOpen)
		{
			Close();
		}
	}

public:
	using FOnGenerateWidget = typename TSlateDelegates<DataType>::FOnGenerateWidget;
	using FOnGenerateRow = typename SListView<DataType>::FOnGenerateRow;
	using FOnSelectionChanged = typename SListView<DataType>::FOnSelectionChanged;
	SLATE_BEGIN_ARGS(SModioCustomComboBox) : _ContentPadding(FMargin()) {}
	// OnGenerateWidget
	SLATE_ARGUMENT(FMargin, ContentPadding)
	SLATE_ARGUMENT(bool, bUseRoundedCorners)
	SLATE_ARGUMENT(TOptional<FModioWidgetBorderStyle>, MenuBorderStyle)
	SLATE_ARGUMENT(FModioUIColorRef, BackgroundColor)
	SLATE_EVENT(FOnGenerateWidget, OnGetButtonContent)
	SLATE_EVENT(FOnGenerateRow, OnGenerateOptionWidget)
	SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
	SLATE_ARGUMENT(TArray<DataType>*, ItemsSource)
	SLATE_ARGUMENT(EMenuPlacement, MenuPlacement)
	SLATE_ARGUMENT(FSlateSound, PressedSound)
	SLATE_ARGUMENT(FSlateSound, HoveredSound)
	SLATE_END_ARGS()

	bool bIsOpen = false;
	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
				  FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
				  bool bParentEnabled) const override
	{
		if (MyOptionalRetainerWidget)
		{
			if (BorderRadius)
			{
				MyOptionalRetainerWidget->GetEffectMaterial()->SetScalarParameterValue(
					"Radius", BorderRadius / (bIsOpen ? 1 + UnderlyingData->Num() : 1));
			}
			MyOptionalRetainerWidget->GetEffectMaterial()->SetScalarParameterValue("Hovered", bIsHovered ? 1 : 0);
		}
		if (bIsOpen && MyListView)
		{
			FVector2D RealSize =
				FVector2D(AllottedGeometry.GetLocalSize().X,
						  ComputeDesiredSize(CachedScaleMultiplier).Y + MyListView->GetDesiredSize().Y);
			OutDrawElements.QueueDeferredPainting(FSlateWindowElementList::FDeferredPaint(
				ChildSlot.GetWidget(), Args.WithNewParent(this),
				AllottedGeometry.MakeChild(RealSize, ComputeLocalTransform()), InWidgetStyle, bParentEnabled));
			return LayerId;
		}
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
										bParentEnabled);
	}
	void Construct(const FArguments& FArgs)
	{
		OnGetButtonContent = FArgs._OnGetButtonContent;
		OnGenerateOptionWidget = FArgs._OnGenerateOptionWidget;
		OnSelectionChangedDelegate = FArgs._OnSelectionChanged;
		UnderlyingData = FArgs._ItemsSource;
		BackgroundColor = FArgs._BackgroundColor;
		MenuPlacement = FArgs._MenuPlacement;
		ContentPadding = FArgs._ContentPadding;
		MyContent = CreateButtonContentWidget();
		MyListView = CreateListViewWidget();
		PressedSound = FArgs._PressedSound;
		HoveredSound = FArgs._HoveredSound;

		// clang-format off
		
		//Possibly wrap in a size box with desired size set to the maximum of the button content or the listview width
		TSharedPtr<SOverlay> Outer = 
			SNew(SOverlay)
			+SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(MyBackground, SImage)
				.Image(this, &SModioCustomComboBox::GetBackgroundBrush)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Expose(ButtonSlot)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					MyContent.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.Expose(ListSlot)
				.AutoHeight()
				[
					MyListView.ToSharedRef()
				]
			];

		if (MyListView)
		{
			MyListView->SetScrollbarVisibility(EVisibility::Collapsed);
		}
		
		if (FArgs._bUseRoundedCorners && FArgs._MenuBorderStyle.IsSet()
			&& !FArgs._MenuBorderStyle->MaskMaterial.IsNull())
		{

			ChildSlot
			[
				SAssignNew(MyOptionalRetainerWidget, SRetainerWidget)
				.Content()
				[
					Outer.ToSharedRef()
				]
			];
			
			MyOptionalRetainerWidget->SetEffectMaterial(FArgs._MenuBorderStyle->MaskMaterial.LoadSynchronous());
			MyOptionalRetainerWidget->SetTextureParameter("WidgetTexture");
			UMaterialInstanceDynamic* BorderMaterialInstance = MyOptionalRetainerWidget->GetEffectMaterial();
			ApplyModioBorderStyle(BorderMaterialInstance, &FArgs._MenuBorderStyle.GetValue());
			BorderRadius = FArgs._MenuBorderStyle->CornerRadius;
		}
		else
		{
			ChildSlot
			[
				Outer.ToSharedRef()
			];
		}
		// clang-format on
	}

	void Open()
	{
		bIsOpen = true;
		MyContent = CreateButtonContentWidget();
		MyListView = CreateListViewWidget();
		if (ButtonSlot)
		{
			ButtonSlot->AttachWidget(MyContent.ToSharedRef());
		}
		if (ListSlot)
		{
			ListSlot->AttachWidget(MyListView.ToSharedRef());
			if (MyListView)
			{
				MyListView->SetScrollbarVisibility(EVisibility::Collapsed);
			}
		}
	}
	void Toggle()
	{
		bIsOpen = !bIsOpen;
		MyContent = CreateButtonContentWidget();
		MyListView = CreateListViewWidget();
		if (ButtonSlot)
		{
			ButtonSlot->AttachWidget(MyContent.ToSharedRef());
		}
		if (ListSlot)
		{
			if (bIsOpen)
			{
				ListSlot->AttachWidget(MyListView.ToSharedRef());
				if (MyListView)
				{
					MyListView->SetScrollbarVisibility(EVisibility::Collapsed);
				}
			}
			else
			{
				ListSlot->DetachWidget();
				FSlateApplication::Get().SetKeyboardFocus(MyContent);
			}
		}
		
	}
	void Close()
	{
		bIsOpen = false;
		MyContent = CreateButtonContentWidget();
		MyListView = CreateListViewWidget();
		if (ButtonSlot)
		{
			ButtonSlot->AttachWidget(MyContent.ToSharedRef());
		}
		if (ListSlot)
		{
			ListSlot->DetachWidget();
		}
		FSlateApplication::Get().SetKeyboardFocus(MyContent);
	}

	TSharedPtr<SWidget> GetContent()
	{
		return MyContent;
	}

	void SetBorderStyle(TOptional<FModioWidgetBorderStyle> NewStyle)
	{
		if (NewStyle.IsSet() && NewStyle->MaskMaterial.IsValid())
		{
			if (MyOptionalRetainerWidget)
			{
				UMaterialInstanceDynamic* BorderMaterialInstance = MyOptionalRetainerWidget->GetEffectMaterial();
				ApplyModioBorderStyle(BorderMaterialInstance, &NewStyle.GetValue());
				BorderRadius = NewStyle->CornerRadius;
			}
		}
	}
	void SetMenuPlacement(EMenuPlacement NewPlacement)
	{
		MenuPlacement = NewPlacement;
	}
	int32 GetSelectedItemIndex()
	{
		return CurrentSelectionIndex.IsSet() ? CurrentSelectionIndex.GetValue() : -1;
	}
	void SetSelectedItemIndex(int32 Index)
	{
		if (!MyListView)
		{
			return;
		}
		if (Index >= 0 && Index < UnderlyingData->Num())
		{
			
			MyListView->SetSelection((*UnderlyingData)[Index]);
		}
		else
		{
			MyListView->ClearSelection();
		}
	}
	void Refresh()
	{
		MyContent = CreateButtonContentWidget();
		if (ButtonSlot)
		{
			ButtonSlot->AttachWidget(MyContent.ToSharedRef());
		}
		if (MyListView)
		{
			MyListView->RebuildList();
		}
	}

protected:
	EMenuPlacement MenuPlacement = EMenuPlacement::MenuPlacement_ComboBox;
	float CachedScaleMultiplier;
	virtual void CacheDesiredSize(float InLayoutScaleMultiplier) override
	{
		CachedScaleMultiplier = InLayoutScaleMultiplier;
		SCompoundWidget::CacheDesiredSize(InLayoutScaleMultiplier);
	}
	virtual FVector2D ComputeDesiredSize(float InLayoutScaleMultiplier) const override
	{
		FVector2D ComputedDesiredSize = SCompoundWidget::ComputeDesiredSize(InLayoutScaleMultiplier);
		if (bIsOpen && MyListView)
		{
			ComputedDesiredSize = ComputedDesiredSize - FVector2D(0, MyListView->GetDesiredSize().Y);
		}
		return ComputedDesiredSize;
	}
	virtual FSlateLayoutTransform ComputeLocalTransform() const
	{
		if (bIsOpen && MyListView)
		{
			switch (MenuPlacement)
			{
				case EMenuPlacement::MenuPlacement_MatchBottomLeft:
					return FSlateLayoutTransform(FVector2D(0, -MyListView->GetDesiredSize().Y));
					break;
			}
		}
		return FSlateLayoutTransform();
	}
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
	{
		if (!bIsOpen)
		{
			return FNavigationReply::Escape();
		}
		return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
	}
	virtual void OnButtonPressed()
	{
		Toggle();
		if (bIsOpen && MyListView)
		{
			FSlateApplication::Get().SetAllUserFocus(MyListView.ToSharedRef(), EFocusCause::Navigation);
			FSlateApplication::Get().PlaySound(PressedSound);
		}
	}
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (IsEnabled() &&
			FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
		{
			TArray<DataType> SelectedItems;
			if (MyListView && MyListView->GetSelectedItems(SelectedItems))
			{
				DataType Item = SelectedItems[0];
				if (UnderlyingData && Item != nullptr)
				{
					int32 SelectedItemIndex = UnderlyingData->Find(Item);
					if (SelectedItemIndex != INDEX_NONE)
					{
						// check here if the user selected a different item before assigning and emitting an event
						CurrentSelectionIndex = SelectedItemIndex;
						if (OnSelectionChangedDelegate.IsBound())
						{
							FSlateApplication::Get().PlaySound(PressedSound);
							OnSelectionChangedDelegate.Execute(Item, ESelectInfo::Direct);
						}
						if (bIsOpen)
						{
							Close();
						}
					}
				}
			}
			return FReply::Handled();
		}
		return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

	virtual void OnSelectionChanged(DataType Item, ESelectInfo::Type SelectionType)
	{
		// Ensure that the proposed selection is different
		if (SelectionType == ESelectInfo::OnNavigation)
		{
			FSlateApplication::Get().PlaySound(HoveredSound);
		}

		if (SelectionType != ESelectInfo::OnNavigation)
		{
			if (UnderlyingData && Item != nullptr)
			{
				int32 SelectedItemIndex = UnderlyingData->Find(Item);
				if (SelectedItemIndex != INDEX_NONE)
				{
					if (CurrentSelectionIndex == SelectedItemIndex)
					{
						return;
					}
					CurrentSelectionIndex = SelectedItemIndex;
					if (OnSelectionChangedDelegate.IsBound())
					{
						OnSelectionChangedDelegate.Execute(Item, SelectionType);
					}
				}
			}
			if (bIsOpen)
			{
				Close();
			}
		}
	}
	virtual const FSlateBrush* GetBackgroundBrush() const
	{
		static FSlateColorBrush BrushInstance(FLinearColor::White);
		BackgroundColor.ApplyToBrush(&BrushInstance);
		return &BrushInstance;
	}
	bool bForceMenuCollapsedForLayoutCalcs = false;
	virtual EVisibility GetMenuVisibility() const
	{
		if (bForceMenuCollapsedForLayoutCalcs)
		{
			return EVisibility::Collapsed;
		}

		return bIsOpen ? EVisibility::Visible : EVisibility::Collapsed;
	}

	bool bIsHovered = false;
	FOnGenerateWidget OnGetButtonContent;
	FOnGenerateRow OnGenerateOptionWidget;
	FOnSelectionChanged OnSelectionChangedDelegate;
	TSharedPtr<SListView<DataType>> MyListView;
	TSharedPtr<SWidget> MyContent;
	TSharedPtr<SImage> MyBackground;
	SVerticalBox::FSlot* ButtonSlot;
	SVerticalBox::FSlot* ListSlot;
	TSharedPtr<SRetainerWidget> MyOptionalRetainerWidget;
	TArray<DataType>* UnderlyingData;
	FModioUIColorRef BackgroundColor;
	TOptional<int32> CurrentSelectionIndex;
	FMargin ContentPadding;
	float BorderRadius = 0;
	FSlateSound PressedSound;
	FSlateSound HoveredSound;
};
