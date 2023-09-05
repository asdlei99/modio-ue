/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/Views/CategoryBrowser/ModioFeaturedMod.h"

#include "ModioUI4Subsystem.h"
#include "Core/ModioModInfoUI.h"
#include "Engine/Engine.h"
#include "ModioUISubsystem.h"
#include "UI/Commands/ModioCommonUICommands.h"

#include "Loc/BeginModioLocNamespace.h"

void UModioFeaturedMod::NativeOnSetDataSource()
{
	Super::NativeOnSetDataSource();

	ModLogoSize = EModioLogoSize::Thumb1280;
	if (DataSource)
	{
		UModioModInfoUI* ModInfo = Cast<UModioModInfoUI>(DataSource);
		if (ModInfo)
		{
			UModioUISubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUISubsystem>();
			Subsystem->RequestLogoDownloadForModID(ModInfo->Underlying.ModId, ModLogoSize);

			// Some of this logic perhaps would be better on the Subscribe button itself, treat that button as a special
			// case
			/*if (SubscribeButton)
			{
				SubscribeButton->SetLabel(SubscribeLabel);

				if (UModioSubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioSubsystem>())
				{
					// Can hit this function directly because the primary subsystem caches the subscriptions, so we dont
					// have to do it manually in the UI subsystem
					if (Subsystem->QueryUserSubscriptions().Contains(ModInfo->Underlying.ModId))
					{
						bCachedSubscriptionState = true;
						SubscribeButton->SetLabel(UnsubscribeLabel);
					}
				}
			}*/

			if (SubscriptionIndicator)
			{
				SubscriptionIndicator->SetDataSource(DataSource);
			}
		}
	}
}

void UModioFeaturedMod::SubmitNegativeRating()
{
	if (!bIsUserAuthenticated)
	{
		UModioUISubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUISubsystem>();
		if (Subsystem)
		{
			Subsystem->ShowUserAuth();
		}
	}

	UModioModInfoUI* ModInfo = Cast<UModioModInfoUI>(DataSource);
	if (ModInfo)
	{
		UModioSubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioSubsystem>();
		if (Subsystem)
		{
			// Needs additional payload param so we know which type of operation was completed
			Subsystem->SubmitModRatingAsync(
				ModInfo->Underlying.ModId, EModioRating::Negative,
				FOnErrorOnlyDelegateFast::CreateUObject(this, &UModioFeaturedMod::OnRatingSubmissionComplete,
														EModioRating::Negative));
		}
	}
}

void UModioFeaturedMod::SubmitPositiveRating()
{
	if (!bIsUserAuthenticated)
	{
		UModioUISubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUISubsystem>();
		if (Subsystem)
		{
			Subsystem->ShowUserAuth();
		}
	}

	UModioModInfoUI* ModInfo = Cast<UModioModInfoUI>(DataSource);
	if (ModInfo)
	{
		UModioSubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioSubsystem>();
		if (Subsystem)
		{
			// Needs additional payload param so we know which type of operation was completed
			Subsystem->SubmitModRatingAsync(
				ModInfo->Underlying.ModId, EModioRating::Positive,
				FOnErrorOnlyDelegateFast::CreateUObject(this, &UModioFeaturedMod::OnRatingSubmissionComplete,
														EModioRating::Positive));
		}
	}
}

void UModioFeaturedMod::OnRatingSubmissionComplete(FModioErrorCode ec, EModioRating Rating)
{
	if (UModioUISubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUISubsystem>())
	{
		Subsystem->DisplayNotificationParams(UModioNotificationParamsLibrary::CreateRatingNotification(ec, DataSource));
	}
}

void UModioFeaturedMod::SubmitModReport()
{
	UModioModInfoUI* ModInfo = Cast<UModioModInfoUI>(DataSource);
	if (ModInfo)
	{
		UModioUISubsystem* Subsystem = GEngine->GetEngineSubsystem<UModioUISubsystem>();
		if (Subsystem)
		{
			Subsystem->ShowModReportDialog(ModInfo);
		}
	}
}
void UModioFeaturedMod::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (MoreOptionsMenu)
	{
		FModioUIMenuCommandList MenuEntries;
		FModioUIExecuteAction PositiveRatingDelegate;
		// TODO: move these to static UFUNCTIONs someplace?
		PositiveRatingDelegate.BindDynamic(this, &UModioFeaturedMod::SubmitPositiveRating);
		FModioUIExecuteAction NegativeRatingDelegate;
		NegativeRatingDelegate.BindDynamic(this, &UModioFeaturedMod::SubmitNegativeRating);
		FModioUIExecuteAction ReportDelegate;
		ReportDelegate.BindDynamic(this, &UModioFeaturedMod::SubmitModReport);
		MenuEntries.MappedActions.Add(FModioUIMenuEntry {LOCTEXT("RateUp", "Rate Up")},
									  FModioUIAction {PositiveRatingDelegate});
		MenuEntries.MappedActions.Add(FModioUIMenuEntry {LOCTEXT("RateDown", "Rate Down")},
									  FModioUIAction {NegativeRatingDelegate});
		MenuEntries.MappedActions.Add(FModioUIMenuEntry {LOCTEXT("Report", "Report")}, FModioUIAction {ReportDelegate});

		MoreOptionsMenu->SetMenuEntries(MenuEntries);
	}
}

void UModioFeaturedMod::NativeMoreOptionsClicked()
{
	if (MoreOptionsMenu)
	{
		MoreOptionsMenu->ToggleOpen(true);
	}
}

void UModioFeaturedMod::BuildCommandList(TSharedRef<FUICommandList> CommandList)
{
	Super::BuildCommandList(CommandList);

	CommandList->MapAction(
		FModioCommonUICommands::Get().MoreOptions,
		FUIAction(FExecuteAction::CreateUObject(this, &UModioFeaturedMod::NativeMoreOptionsClicked)));
}

// This should be 'while in the focus path'
FReply UModioFeaturedMod::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UModioFeaturedMod::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	TileActiveFrame->GetRenderOpacity() > 0.0f ? bShouldPlayAnimation = true : bShouldPlayAnimation = false;
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void UModioFeaturedMod::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	TileActiveFrame->GetRenderOpacity() > 0.0f ? bShouldPlayAnimation = true : bShouldPlayAnimation = false;
	Super::NativeOnFocusLost(InFocusEvent);
}

void UModioFeaturedMod::NativeOnItemSelectionChanged(bool bIsSelected)
{
	Super::NativeOnItemSelectionChanged(bIsSelected);
	
	bCurrentSelectionState = bIsSelected;

	if (bCurrentSelectionState)
	{
		PlayHoverAnimation(true);
	}

	if (SubscribeButton)
	{
		SubscribeButton->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (ModName)
	{
		ModName->SetVisibility(bIsSelected ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (MoreOptionsMenu)
	{
		MoreOptionsMenu->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (TileActiveFrame)
	{
		TileActiveFrame->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UModioFeaturedMod::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);

	UModioModInfoUI* ModInfo = Cast<UModioModInfoUI>(DataSource);

	if (ModName && IsValid(ModInfo))
	{
		FString modName = ModInfo->Underlying.ProfileName;
		modName = TruncateLongModName(modName, ModName, truncateDivider);
		ModName->SetText(FText::FromString(modName));
	}
}

void UModioFeaturedMod::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) 
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UModioFeaturedMod::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) 
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UModioFeaturedMod::NativeOnModLogoDownloadCompleted(FModioModID ModID, FModioErrorCode ec,
														 TOptional<FModioImageWrapper> Image, EModioLogoSize LogoSize)
{
	// Load image via call to Super if logo has not been set
	if (!CurrentLogoSize.IsSet())
	{
		Super::NativeOnModLogoDownloadCompleted(ModID, ec, Image, LogoSize);
		return;
	}
	else if (UModioModInfoUI* ModInfo = GetAsModInfoUIObject())
	{
		// Load image via call to Super if we're improving the image quality for the specified mod
		if (ModID == ModInfo->Underlying.ModId && (LogoSize > *CurrentLogoSize))
		{
			Super::NativeOnModLogoDownloadCompleted(ModID, ec, Image, LogoSize);
			return;
		}
	}
	// No need to load image
	IModioUIMediaDownloadCompletedReceiver::NativeOnModLogoDownloadCompleted(ModID, ec, Image, LogoSize);
}

FReply UModioFeaturedMod::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bCurrentSelectionState)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	OnModTilePressed.Broadcast(this);
	return FReply::Unhandled();
}

#include "Loc/EndModioLocNamespace.h"