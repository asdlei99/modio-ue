/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/CommonComponents/ModioTabController.h"

void UModioTabController::NativeOnCurrentTabChanged(int64 TabIndex, UObject* AssociatedItem)
{
	bRoutedOnCurrentTabChanged = true;
	OnCurrentTabChanged.Broadcast(TabIndex, AssociatedItem);
}

void UModioTabController::HandleItemSelectionChanged(UObject* Item)
{
	bRoutedOnCurrentTabChanged = false;
	NativeOnCurrentTabChanged(TabButtons->GetIndexForItem(Item), Item);
	checkf(bRoutedOnCurrentTabChanged,
		   TEXT("Native overrides of NativeOnCurrentTabChanged should call Super::NativeOnCurrentTabChanged to ensure "
				"events are routed to Blueprint"));
}

void UModioTabController::HandleItemClicked(UObject* Item) {}

void UModioTabController::NativeConstruct()
{
	Super::NativeConstruct();
	if (TabButtons)
	{
		TabButtons->OnItemSelectionChanged().AddUObject(this, &UModioTabController::HandleItemSelectionChanged);
		TabButtons->OnItemClicked().AddUObject(this, &UModioTabController::HandleItemClicked);
	}

	if (NavigatePreviousHint)
	{
		NavigatePreviousHint->GetInputModeVisibilityDelegate().BindDynamic(
			this, &UModioTabController::GetInputHintVisibility);
	}

	if (NavigateNextHint)
	{
		NavigateNextHint->GetInputModeVisibilityDelegate().BindDynamic(this,
																	   &UModioTabController::GetInputHintVisibility);
	}
}

ESlateVisibility UModioTabController::GetInputHintVisibility(EModioUIInputMode InputMode)
{
	if (InputMode == EModioUIInputMode::Mouse || InputMode == EModioUIInputMode::Unknown)
	{
		return ESlateVisibility::Collapsed;
	}
	else
	{
		return ESlateVisibility::SelfHitTestInvisible;
	}
}

void UModioTabController::SetTabItems(TArray<UObject*> NewItems)
{
	if (TabButtons)
	{
		TabButtons->ClearListItems();
		TabButtons->SetListItems(NewItems);
	}
}

void UModioTabController::SetCurrentTab(int64 TabIndex)
{
	TabButtons->SetSelectedIndex(TabIndex);
}

void UModioTabController::NextTab()
{
	SetCurrentTab((TabButtons->GetIndexForItem(TabButtons->GetSelectedItem()) + 1) % TabButtons->GetNumItems());
}

void UModioTabController::PrevTab()
{
	if (TabButtons->GetIndexForItem(TabButtons->GetSelectedItem()) == 0)
	{
		SetCurrentTab(TabButtons->GetNumItems() - 1);
	}
	else
	{
		SetCurrentTab(TabButtons->GetIndexForItem(TabButtons->GetSelectedItem()) - 1);
	}
}
