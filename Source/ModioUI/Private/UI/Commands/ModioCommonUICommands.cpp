/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/Commands/ModioCommonUICommands.h"
#include "Framework/Commands/UICommandInfo.h"

#include "Loc/BeginModioLocNamespace.h"

FModioUIMenuEntry UModioUICommandInfoLibrary::PreviousPageCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().PreviousPage->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::NextPageCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().NextPage->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::MoreOptionsCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().MoreOptions->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::ConfirmCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().Confirm->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::RefineSearchCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().RefineSearch->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::BackCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().Back->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::DownloadQueueCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().DownloadQueue->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::CollapseCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().Collapse->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::LogOutCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().LogOut->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::RateUpCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().RateUp->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::RateDownCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().RateDown->GetLabel()};
}

FModioUIMenuEntry UModioUICommandInfoLibrary::ReportCommand()
{
	return FModioUIMenuEntry {FModioCommonUICommands::Get().Report->GetLabel()};
}

FUIAction FModioCommonUICommands::GetNullAction()
{
	return FUIAction(FExecuteAction::CreateLambda([]() {}), FCanExecuteAction::CreateLambda([]() { return false; }));
}

void FModioCommonUICommands::RegisterCommands()
{
	// The input chords passed here need to map to our custom keys
	// The input settings for the plugin needs to map an arbitrary number of input chords to the custom key

	FUICommandInfo::MakeCommandInfo(this->AsShared(), PreviousPage, "Previous", LOCTEXT("Previous.Label", "Prev"),
									LOCTEXT("Previous.Desc", "Displays the previous page"), FSlateIcon(),
									EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::Previous, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(this->AsShared(), NextPage, "Next", LOCTEXT("Next.Label", "Next"),
									LOCTEXT("Next.Desc", "Displays the next page"), FSlateIcon(),
									EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::Next, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), MoreOptions, "MoreOptions", LOCTEXT("MoreOptions.Label", "More Options"),
		LOCTEXT("MoreOptions.Desc", "Displays additional options for the current item"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::MoreOptions, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), Confirm, "Confirm", LOCTEXT("Confirm.Label", "Confirm"),
		LOCTEXT("Confirm.Desc", "Confirms or performs the default action for the current item"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::Confirm, EModifierKey::None));
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), Subscribe, "Subscribe", LOCTEXT("Subscribe.Label", "Subscribe"),
		LOCTEXT("Subscribe.Desc", "Triggers a subscription or unsubscription for the current context"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::Subscribe, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), RefineSearch, "RefineSearch", LOCTEXT("RefineSearch.Label", "Refine Search"),
		LOCTEXT("Refine Search.Desc", "Filters the current set of results"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::RefineSearch, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(this->AsShared(), Back, "Back", LOCTEXT("Back.Label", "Back"),
									LOCTEXT("Back.Desc", "Returns to the previous screen or exits the browser"),
									FSlateIcon(), EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::Back, EModifierKey::None));
	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), DownloadQueue, "DownloadQueue", LOCTEXT("DownloadQueue.Label", "Download Queue"),
		LOCTEXT("DownloadQueue.Desc", "Displays the user profile and download queue"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::DownloadQueue, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), EnableDisableMod, "EnableDisableMod", LOCTEXT("EnableDisableMod.Label", "Enable Or Disable Mod"),
		LOCTEXT("EnableDisable.Desc", "Enable or disable selected mod"), FSlateIcon(),
		EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::EnableDisableMod, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), Collapse, "Collapse", LOCTEXT("Collapse.Label", "Collapse"),
		LOCTEXT("Collapse.Desc", "Toggle collapse tag category"), FSlateIcon(),
									EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::Collapse, EModifierKey::None));

		FUICommandInfo::MakeCommandInfo(this->AsShared(), LogOut, "LogOut", LOCTEXT("LogOut.Label", "LogOut"),
									LOCTEXT("LogOut.Desc", "Logout from modio"), FSlateIcon(),
									EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::LogOut, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(this->AsShared(), RateUp, "RateUp", LOCTEXT("RateUp.Label", "RateUp"),
									LOCTEXT("RateUp.Desc", "Rate up a mod"), FSlateIcon(),
									EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::RateUp, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(
		this->AsShared(), RateDown, "RateDown", LOCTEXT("RateDown.Label", "RateDown"), LOCTEXT("RateDown.Desc", "Rate down a mod"),
		FSlateIcon(), EUserInterfaceActionType::Button, FInputChord(FModioInputKeys::RateDown, EModifierKey::None));

	FUICommandInfo::MakeCommandInfo(this->AsShared(), Report, "Report", LOCTEXT("Report.Label", "Report"),
									LOCTEXT("Report.Desc", "Report a mod"), FSlateIcon(),
									EUserInterfaceActionType::Button,
									FInputChord(FModioInputKeys::Report, EModifierKey::None));
}

#include "Loc/EndModioLocNamespace.h"