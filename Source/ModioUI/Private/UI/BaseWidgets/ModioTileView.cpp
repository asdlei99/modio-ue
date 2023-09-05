/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/ModioTileView.h"
#include "UI/CommonComponents/ModioModTile.h"
#include "UI/CommonComponents/ModioModTileBase.h"
#include "UI/BaseWidgets/Slate/SModioObjectTableRow.h"

void UModioTileView::OnItemHoveredChanged(UObject* Item, bool bNewSelectionState)
{
	UModioUI4Subsystem* subsystem = GEngine->GetEngineSubsystem<UModioUI4Subsystem>();
	if (IsValid(subsystem) && subsystem->GetCurrentFocusTarget()) 
	{
		UModioModTile* modTile = Cast<UModioModTile>(subsystem->GetCurrentFocusTarget());
		if (IsValid(modTile) && !modTile->AllowMouseHoverFocus())
		{
			return;
		}
	}
	MyDerivedTileView->SetScrollToTarget(false);
	FSlateApplication::Get().SetUserFocus(0, MyDerivedTileView->WidgetFromItem(Item)->AsWidget());
	UTileView::SetItemSelection(Item, bNewSelectionState);
	//UTileView::RequestNavigateToItem(Item);
}

void UModioTileView::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyDerivedTileView)
	{
		MyDerivedTileView->SetAllowPartialItems(AllowPartialItems);
		MyDerivedTileView->SetCenterBasedOnPanelItems(CenterPanelItems);
	}
}

void UModioTileView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyDerivedTileView.Reset();
}

TSharedRef<STableViewBase> UModioTileView::RebuildListWidget()
{
	MyDerivedTileView = TSharedPtr<SModioTileView<UObject*>>(ConstructTileView<SModioTileView>());
	MyDerivedTileView->SetVisibility(EVisibility::SelfHitTestInvisible);
	OnItemIsHoveredChanged().AddUObject(this, &UModioTileView::OnItemHoveredChanged);
	MyDerivedTileView->SetScrollbarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(ScrollbarVisibility));
	return MyDerivedTileView.ToSharedRef();
}

void UModioTileView::OnSelectionChangedInternal(NullableItemType FirstSelectedItem)
{
	Super::OnSelectionChangedInternal(FirstSelectedItem);
}

UUserWidget& UModioTileView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
														   const TSharedRef<STableViewBase>& OwnerTable)
{
	if (bZoomSelectedEntryWidget)
	{
		UUserWidget& GeneratedWidget =
			GenerateTypedEntry<UUserWidget, SModioObjectTableRow<UObject*>>(DesiredEntryClass, OwnerTable);
		TSharedRef<SModioObjectTableRow<UObject*>> ConcreteSlateWidget =
			StaticCastSharedRef<SModioObjectTableRow<UObject*>>(GeneratedWidget.TakeWidget());
		return GeneratedWidget;
	}
	else
	{
		return GenerateTypedEntry(DesiredEntryClass, OwnerTable);
	}
}

void UModioTileView::SetConsumeMouseWheel(EConsumeMouseWheel NewValue)
{
	ConsumeMouseWheel = NewValue;
	if (MyDerivedTileView)
	{
		MyDerivedTileView->SetConsumeMouseWheel(NewValue);
	}
}

float UModioTileView::GetScrollOffset()
{
	return MyTileView->GetScrollOffset();
}

void UModioTileView::SetAllowPartialItems(bool bAllowPartialItems)
{
	AllowPartialItems = bAllowPartialItems;
	if (MyDerivedTileView)
	{
		MyDerivedTileView->SetAllowPartialItems(bAllowPartialItems);
	}
}
void UModioTileView::SetCenterPanelItems(bool bCenterPanelItems)
{
	CenterPanelItems = bCenterPanelItems;
	if (MyDerivedTileView)
	{
		MyDerivedTileView->SetCenterBasedOnPanelItems(bCenterPanelItems);
	}
}
