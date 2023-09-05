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

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "ModioUI4Subsystem.h"
#include "UI/BaseWidgets/ModioUserWidgetBase.h"
#include "UI/BaseWidgets/Slate/SModioWidgetCarouselBase.h"
#include "TimerManager.h"
#include "ModioWidgetCarousel.generated.h"

/**
* Base class definition of a Modio carousel entry wrapper
**/
UCLASS()
class MODIOUI_API UModioWidgetCarouselEntryWrapper : public UWidget
{
	GENERATED_BODY()
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
};

/**
* Base class definition of a Modio carousel that references multiple widgets
**/
UCLASS()
class MODIOUI_API UModioWidgetCarousel : public UWidget
{
	GENERATED_BODY()
protected:
	TArray<UObject*> ItemsSource;
	TSharedPtr<SModioWidgetCarouselBase<UObject*>> MyCarousel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListEntries,
			  meta = (DesignerRebuild, AllowPrivateAccess = true, MustImplement = UserListEntry))
	TSubclassOf<UModioUserWidgetBase> EntryWidgetClass;

	virtual TSharedRef<SWidget> OnGenerateWidget(UObject* Item);

	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// These may need to be reassigned after each scroll on the internal widget;

	UPROPERTY(Transient, BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	UModioWidgetCarouselEntryWrapper* LeftCarouselWidget;
	UPROPERTY(Transient, BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	UModioWidgetCarouselEntryWrapper* CenterCarouselWidget;
	UPROPERTY(Transient, BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	UModioWidgetCarouselEntryWrapper* RightCarouselWidget;
	UPROPERTY(Transient, BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	UModioWidgetCarouselEntryWrapper* IncomingWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	float RelativeWidgetSpacing = 1.25f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel")
	EWidgetClipping EntryWidgetClipping;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size")
	EModioWidgetCarouselSizeOverrideType EntryWidgetSizeOverride = EModioWidgetCarouselSizeOverrideType::NoOverride;

	UPROPERTY(
		BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Absolute",
		meta = (EditCondition = "EntryWidgetSizeOverride==EModioWidgetCarouselSizeOverrideType::AbsoluteDimensions"),
		meta = (InlineEditConditionToggle))
	bool bOverrideAbsoluteWidth = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Absolute",
			  meta = (EditCondition = "bOverrideAbsoluteWidth"))
	float EntryWidgetWidthOverride = 0.0;

	UPROPERTY(
		BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Absolute",
		meta = (EditCondition = "EntryWidgetSizeOverride==EModioWidgetCarouselSizeOverrideType::AbsoluteDimensions"),
		meta = (InlineEditConditionToggle))
	bool bOverrideAbsoluteHeight = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Absolute",
			  meta = (EditCondition = "bOverrideAbsoluteHeight"))
	float EntryWidgetHeightOverride = 0.0;

	UPROPERTY(
		BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Relative Layout Scale",
		meta = (EditCondition = "EntryWidgetSizeOverride==EModioWidgetCarouselSizeOverrideType::RelativeLayoutScale"),
		meta = (InlineEditConditionToggle))
	bool bOverrideRelativeLayoutWidth = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Relative Layout Scale",
			  meta = (EditCondition = "bOverrideRelativeLayoutWidth"))
	float EntryWidgetRelativeWidthOverride = 1.0;

	UPROPERTY(
		BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Relative Layout Scale",
		meta = (EditCondition = "EntryWidgetSizeOverride==EModioWidgetCarouselSizeOverrideType::RelativeLayoutScale"),
		meta = (InlineEditConditionToggle))
	bool bOverrideRelativeLayoutHeight = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Carousel|Widget Size|Relative Layout Scale",
			  meta = (EditCondition = "bOverrideRelativeLayoutHeight"))
	float EntryWidgetRelativeHeightOverride = 1.0;

	virtual void NativeLeftWidgetClickedHandler()
	{
		MyCarousel->ScrollLeft();
	}
	virtual void NativeCenterWidgetClickedHandler() {}
	virtual void NativeRightWidgetClickedHandler()
	{
		MyCarousel->ScrollRight();
	}

	void LeftWidgetClickedHandler(TSharedPtr<SModioWidgetCarouselEntry> Widget, UObject* ItemEntry)
	{
		NativeLeftWidgetClickedHandler();
	}
	void CenterWidgetClickedHandler(TSharedPtr<SModioWidgetCarouselEntry> Widget, UObject* ItemEntry)
	{
		NativeCenterWidgetClickedHandler();
	}
	void RightWidgetClickedHandler(TSharedPtr<SModioWidgetCarouselEntry> Widget, UObject* ItemEntry)
	{
		NativeRightWidgetClickedHandler();
	}

public:
	/**
	* Update the object elements contained in the carousel
	* @param Items Array of objects to update the carousel 
	**/
	UFUNCTION(BlueprintCallable, Category = "ModioWidgetCarousel")
	void SetItems(TArray<UObject*> Items);

	/**
	* Move the focus to the left
	**/
	UFUNCTION(BlueprintCallable, Category = "ModioWidgetCarousel")
	void ScrollLeft()
	{
		MyCarousel->ScrollLeft();
	}

	/**
	* Move the focus to the right
	**/
	UFUNCTION(BlueprintCallable, Category = "ModioWidgetCarousel")
	void ScrollRight()
	{
		MyCarousel->ScrollRight();
	}

	UFUNCTION(BlueprintCallable, Category = "ModioWidgetCarousel")
	void SetFocusToCurrentElement() 
	{
		FTimerHandle timer;
		if (!MyCarousel  || (!MyCarousel->SetFocusToContent() && !MyCarousel->HasFocusedDescendants()))
		{
			// In some rare cases GetWorld can return a nullptr
			if (!GetWorld())
			{
				return;
			}

			// Easiest way to ensure the focus is set to some target when this function is called
			// Prevents focus lost on Login/Logout
			UModioUI4Subsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUI4Subsystem>();
			if (IsValid(Subsystem) && !(Subsystem->GetLastInputDevice() == EModioUIInputMode::Mouse))
			{
				GetWorld()->GetTimerManager().SetTimer(timer, this, &UModioWidgetCarousel::SetFocusToCurrentElement,
													   0.1f, false);
			}
		}
	}
};
