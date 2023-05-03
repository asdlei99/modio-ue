﻿/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/CommonComponents/ModioAuthenticationMethodSelector.h"
#include "Framework/Application/SlateApplication.h"

#include "Loc/BeginModioLocNamespace.h"

TSharedRef<SWidget> UModioAuthenticationMethodSelector::RebuildWidget()
{
	AuthMethods.Empty();
	if (UModioUISettings* Settings = UModioUISettings::StaticClass()->GetDefaultObject<UModioUISettings>())
	{
		if (UClass* AuthProviderClass = Settings->AuthenticationDataProvider.Get())
		{
			UObject* TmpProvider = NewObject<UObject>(this, AuthProviderClass);
			TArray<FModioUIAuthenticationProviderInfo> RawAuthMethods =
				IModioUIAuthenticationDataProvider::Execute_GetAuthenticationTypes(TmpProvider);
			if (IModioUIAuthenticationDataProvider::Execute_ShouldOfferEmailAuthentication(TmpProvider))
			{
				AuthMethods.Add(
					MakeShared<FModioUIAuthenticationProviderInfo>(FModioUIAuthenticationProviderInfo::EmailAuth()));
			}
			Algo::Transform(RawAuthMethods, AuthMethods, [](FModioUIAuthenticationProviderInfo ProviderInfo) {
				return MakeShared<FModioUIAuthenticationProviderInfo>(ProviderInfo);
			});
		}
	}
	// Sane default = if no auth provider specified, or one provided that didn't offer any authentication methods, offer
	// email auth so we have *some* authentication method available
	if (AuthMethods.Num() == 0)
	{
		AuthMethods.Add(
			MakeShared<FModioUIAuthenticationProviderInfo>(FModioUIAuthenticationProviderInfo::EmailAuth()));
	}

	if (!CancelButton)
	{
		CancelButton = NewObject<UModioRichTextButton>(this);
		CancelButton->SetButtonStyle(ButtonStyle);
		CancelButton->TakeWidget();
		CancelButton->SetLabel(LOCTEXT("Cancel", "Cancel"));
		CancelButton->OnClicked.AddDynamic(this, &UModioAuthenticationMethodSelector::CancelClicked);
	}

	// clang-format off
	SAssignNew(ButtonBox, SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	[
		SAssignNew(MyButtonList, STileView<TSharedPtr<FModioUIAuthenticationProviderInfo>>)
				 .Orientation(Orient_Horizontal)
				 .ListItemsSource(&AuthMethods)
				 .ItemHeight(0)
				 .IsFocusable(false)						// There should be no reason to be able to focus on a slot element
				 .SelectionMode(ESelectionMode::None)
				 .WrapHorizontalNavigation(false)
				 .HandleGamepadEvents(false)
				 .HandleDirectionalNavigation(false)
				 .AllowOverscroll(EAllowOverscroll::No)
				 .ItemWidth(0)
				 .OnGenerateTile_UObject(this, &UModioAuthenticationMethodSelector::OnGenerateButton)
	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.AutoWidth()
	[
		CancelButton->TakeWidget()
	];
		

	MyButtonList->RebuildList();
		
	return ButtonBox.ToSharedRef();
}

FModioUIAuthenticationProviderInfo FModioUIAuthenticationProviderInfo::EmailAuth()
{
	FModioUIAuthenticationProviderInfo Instance;
	Instance.bIsEmailAuthentication = true;
	Instance.ProviderUILabel = LOCTEXT("Email", "Email");
	return Instance;
}

#include "Loc/EndModioLocNamespace.h"