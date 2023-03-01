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

#include "Components/Image.h"
#include "Core/Input/InputDeviceTypes.h"
#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "Types/ModioImageWrapper.h"
#include "UObject/StrongObjectPtr.h"

#include "ModioImage.generated.h"

/**
 *
 */
UCLASS()
class MODIOUI_API UModioImage : public UImage
{
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties();


	virtual void ImageLoadHandler(UTexture2DDynamic* Texture);

	UFUNCTION()
	virtual void ImageLoadHandler(UTexture2DDynamic* Texture, class UMaterialInterface* Material = nullptr,
								  FName ImageParameterName = "");
	
	UPROPERTY()
	class UMaterialInstanceDynamic* CachedMaterial;

public:
	UModioImage(const FObjectInitializer& ObjectInitializer);

	virtual void SetBrushFromMaterial(UMaterialInterface* Material) override;

	UFUNCTION(BlueprintCallable, Category = "ModioImage")
	virtual void DisplayImageWithMaterial(UTexture2DDynamic* Texture, UMaterialInterface* Material,
										  FName ImageParameterName);

	UFUNCTION(BlueprintCallable, Category = "ModioImage")
	virtual void LoadImageFromFileAsync(FModioImageWrapper ImageLoader);

	UFUNCTION(BlueprintCallable, Category = "ModioImage")
	void LoadImageFromFileWithMaterialAsync(FModioImageWrapper ImageLoader, UMaterialInterface* Material,
											FName ImageParameterName);
};
