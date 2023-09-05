﻿/*
 *  Copyright (C) 2023 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue/blob/main/LICENSE>)
 *
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "Styling/SlateTypes.h"
#include "ModioCommonExpandableAreaStyle.generated.h"

class UModioCommonTextStyle;

/**
 * @brief The style of the Border within the Mod.io Common UI styling system
 */
UCLASS(Abstract, Blueprintable, ClassGroup = "UI", meta = (Category = "Mod.io Common UI"))
class MODIOUI5_API UModioCommonExpandableAreaStyle : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod.io Common UI|Style")
	TSubclassOf<UModioCommonTextStyle> NormalLabelTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod.io Common UI|Style")
	TSubclassOf<UModioCommonTextStyle> SelectedLabelTextStyle;
	
	UPROPERTY(EditAnywhere, Category = "Mod.io Common UI|Style")
	FExpandableAreaStyle Style;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mod.io Common UI|Style")
	FSlateBrush BorderBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mod.io Common UI|Style")
	FSlateColor BorderColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Expansion")
	bool bIsExpandedByDefault;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Expansion")
	float MaxHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Expansion")
	FMargin HeaderPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Expansion")
	FMargin AreaPadding;
};