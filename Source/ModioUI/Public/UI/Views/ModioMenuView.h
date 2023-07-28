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

#include "CoreMinimal.h"
#include "UI/BaseWidgets/ModioUserWidgetBase.h"

#include "ModioMenuView.generated.h"

/**
 *
 */
UCLASS()
class MODIOUI_API UModioMenuView : public UModioUserWidgetBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	UUserWidget* CachedMenuTitleContentWidget;

	/// @brief Displayed on the associated menu bar
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets")
	FText MenuName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> TitleContentClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets")
	bool bShouldShowTopNavBar = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets")
	bool bShouldShowBackButton = true;

public:

	const FText& GetName();
	// function here to get the index inside our view switcher?

	virtual UUserWidget* GetMenuTitleContent();
	virtual bool ShouldShowSearchButtonForMenu();
	virtual bool ShouldShowBackButtonForMenu();
	virtual bool ShouldShowTopNavBar();
};
