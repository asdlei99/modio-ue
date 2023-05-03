/*
 *  Copyright (C) 2021 mod.io Pty Ltd. <https://mod.io>
 *
 *  This file is part of the mod.io UE4 Plugin.
 *
 *  Distributed under the MIT License. (See accompanying file LICENSE or
 *   view online at <https://github.com/modio/modio-ue4/blob/main/LICENSE>)
 *
 */

#include "UI/BaseWidgets/ModioRoundedImage.h"
#include "UI/Styles/ModioUIStyleSet.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void UModioRoundedImage::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (UMaterialInterface* Material = MaterialToUse.ResolveReference(nullptr, FName(this->GetFullName())))
	{
		// Because we're setting the brush's resource object, we want to make sure that we aren't referencing a material
		// with another outer

		// We don't want to modify the outer of the material we get back, because that material is
		// shared and it's outer is the styleset that stores it, so we duplicate it as a dynamic instance instead, so it
		// won't be saved and we won't get errors about external packages

		if (UMaterialInstanceDynamic* AsDynamicMaterial = Cast<UMaterialInstanceDynamic>(Material))
		{
			ImageMaterial = UMaterialInstanceDynamic::Create(AsDynamicMaterial->Parent, this);
			ImageMaterial->CopyInterpParameters(AsDynamicMaterial);
			SetBrushFromMaterial(ImageMaterial);
		}
		else
		{
			ImageMaterial = UMaterialInstanceDynamic::Create(Material, this);
			SetBrushFromMaterial(ImageMaterial);
		}
	}
}

void UModioRoundedImage::LoadImageFromFileAsync(FModioImageWrapper ImageLoader)
{
	if (UMaterialInterface* Material = MaterialToUse.ResolveReference(nullptr, FName(this->GetFullName())))
	{
		CachedReferencedMaterial = Material;
		LoadImageFromFileWithMaterialAsync(ImageLoader, Material, TextureParameterName);
	}
	else
	{
		Super::LoadImageFromFileAsync(ImageLoader);
	}
}
