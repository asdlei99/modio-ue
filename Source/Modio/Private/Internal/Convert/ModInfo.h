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
#include "Internal/Convert/FileMetadata.h"
#include "Internal/Convert/Metadata.h"
#include "Internal/Convert/ModStats.h"
#include "Internal/Convert/ModTag.h"
#include "Internal/Convert/URLList.h"
#include "Internal/Convert/User.h"
#include "Internal/ModioConvert.h"
#include "ModioSDK.h"
#include "Types/ModioModInfo.h"

FORCEINLINE FModioModInfo ToUnreal(const Modio::ModInfo& In)
{
	FModioModInfo Out;
	Out.ModId = ToUnreal(In.ModId);
	Out.ProfileName = ToUnreal(In.ProfileName);
	Out.ProfileSummary = ToUnreal(In.ProfileSummary);
	Out.ProfileDescription = ToUnreal(In.ProfileDescription);
	Out.ProfileDescriptionPlaintext = ToUnreal(In.ProfileDescriptionPlaintext);
	Out.ProfileURL = ToUnreal(In.ProfileURL);
	Out.ProfileSubmittedBy = ToUnreal(In.ProfileSubmittedBy);
	Out.ProfileDateAdded = ToUnrealDateTime(In.ProfileDateAdded);
	Out.ProfileDateUpdated = ToUnrealDateTime(In.ProfileDateUpdated);
	Out.ProfileDateLive = ToUnrealDateTime(In.ProfileDateLive);
	Out.ProfileMaturityOption = ToUnreal(In.ProfileMaturityOption);
	Out.MetadataBlob = FString(In.MetadataBlob.c_str()); // Converting verbatim rather than via TCHAR as ToUnreal does
	Out.FileInfo = ToUnreal(In.FileInfo);
	Out.MetadataKvp = ToUnreal<FModioMetadata>(In.MetadataKvp);
	Out.Tags = ToUnreal<FModioModTag>(In.Tags);
	Out.NumGalleryImages = ToUnreal(In.NumGalleryImages);
	Out.YoutubeURLs = ToUnreal(In.YoutubeURLs);
	Out.SketchfabURLs = ToUnreal(In.SketchfabURLs);
	Out.Stats = ToUnreal(In.Stats);
	return Out;
}