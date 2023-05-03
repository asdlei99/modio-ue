/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/ModioListView.h"

void UModioListView::SynchronizeProperties()
{
	Super::SynchronizeProperties();
#if WITH_EDITORONLY_DATA
	if (IsDesignTime() && bSimulateSelection)
	{
		SetSelectedIndex(SimulatedSelectionIndex);
	}
#endif
}
