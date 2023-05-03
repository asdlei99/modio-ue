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

#include "Engine/DataAsset.h"
#include "IModioUIStyleRefSerializer.h"
#include "Misc/EngineVersionComparison.h"
#include "PropertyPathHelpers.h"
#include "Styling/ISlateStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"
#include "UI/Interfaces/IModioMenuBackgroundProvider.h"
#include "UI/Styles/ModioColorPresets.h"
#include "UI/Styles/ModioComboBoxStyle.h"
#include "UI/Styles/ModioInputMappingGlyph.h"
#include "UI/Styles/ModioMaterialData.h"
#include "UI/Styles/ModioProceduralBrushParams.h"
#include "UI/Styles/ModioSlateMaterialBrush.h"
#include "UI/Styles/ModioUIColor.h"
#include "UI/Styles/ModioUIWidgetStyleContainer.h"

#include "ModioUIStyleSet.generated.h"

/**
* Strong type struct to store a linear color
**/
USTRUCT(BlueprintType)
struct FModioLinearColor : public FLinearColor
{
	GENERATED_BODY()
};



#if UE_VERSION_NEWER_THAN(5, 0, 0)
	#define MODIO_UE5_REQUESTING_STYLE , const ISlateStyle* RequestingStyle
	#define MODIO_UE5_REQUESTING_STYLE_PARAM MODIO_UE5_REQUESTING_STYLE = nullptr
	#define MODIO_UE5_CONST const
	#define MODIO_UE5_OVERRIDE override
#else
	#define MODIO_UE5_REQUESTING_STYLE
	#define MODIO_UE5_REQUESTING_STYLE_PARAM
	#define MODIO_UE5_CONST
	#define MODIO_UE5_OVERRIDE
#endif

UCLASS(BlueprintType, meta = (MaterialSerialize = "SerializedMaterials", ColorSerialize = "SerializedColors"))
class MODIOUI_API UModioUIStyleSet : public UDataAsset, public ISlateStyle, public IModioUIStyleRefSerializer, public IModioMenuBackgroundProvider
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Style")
	UMaterialInterface* DefaultBackgroundMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Style")
	class UModioColorPresets* ColorPresetsData;

	mutable TSet<FName> MissingResources;

	UPROPERTY(meta = (ShowOnlyInnerProperties))
	TMap<FName, FModioUIColorRef> SerializedColors;

	UPROPERTY(EditAnywhere,
			  meta = (ReadOnlyKeys, EditCondition = "true==false", EditConditionHides, ShowOnlyInnerProperties,
					  EditFixedSize, EditFixedOrder),
			  Category = "Style")
	TMap<FName, FModioUIMaterialRef> SerializedMaterials;

	UPROPERTY()
	TMap<FString, FName> PropertyPathToColorPresetMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ShowOnlyInnerProperties), Category = "Style")
	TArray<class UModioWidgetStyleData*> WidgetStyleAssets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ShowOnlyInnerProperties), Category = "Style")
	TMap<FName, FSlateBrush> NamedBrushes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ShowOnlyInnerProperties), Category = "Style")
	class UModioNamedGlyphsDataAsset* NamedGlyphsAsset;

	// We can probably clear this cache in a ModioUISubsystem::Shutdown call?
	TMap<FName, TSharedPtr<struct FModioSlateMaterialBrush>> MaterialBrushCache;

	UPROPERTY(Transient)
	TMap<FName, UMaterialInterface*> MaterialInstanceCache;

#if UE_VERSION_NEWER_THAN(5, 0, 0)
	const FSlateWidgetStyle* GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName,
													const FSlateWidgetStyle* DefaultStyle,
													bool bWarnIfNotFound) const override;
#else
	const FSlateWidgetStyle* GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const override;
#endif

	void Log(EStyleMessageSeverity Severity, const FText& Message) const override;

	// FSlateStyleSet InternalStyleSet;

	virtual void PostLoad() override;

	friend class FModioUIStyleRefDetailsCustomization;

	virtual TSet<FName> GetStyleKeys() const MODIO_UE5_OVERRIDE;
	virtual FString GetContentRootDir() const MODIO_UE5_OVERRIDE;

	virtual void LogMissingResource(EStyleMessageSeverity Severity, const FText& Message,
									const FName& MissingResource) const MODIO_UE5_OVERRIDE;

public:

	void ClearMaterialCache();

	UModioUIStyleSet(const FObjectInitializer& Initializer);

	TArray<UObject*> GetAssetReferences();

	void RefreshStyleMaterials(FName StyleName);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Style")
	FModioComboBoxStyle DefaultComboBoxStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Style")
	class UModioMaterialData* MaterialData;

	bool bOverridePlatformMaterials = false;

	const FName& GetStyleSetName() const override;

	void GetResources(TArray<const FSlateBrush*>& OutResources) const override;

	TArray<FName> GetEntriesUsingBrush(const FName BrushName) const override;

	float GetFloat(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
				   float DefaultValue = FStyleDefaults::GetFloat() MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	FVector2D GetVector(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
						FVector2D DefaultValue = FStyleDefaults::GetVector2D()
							MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	const FLinearColor& GetColor(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
								 const FLinearColor& DefaultValue = FStyleDefaults::GetColor()
									 MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	const FSlateColor GetSlateColor(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
									const FSlateColor& DefaultValue = FStyleDefaults::GetSlateColor()
										MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	const FMargin& GetMargin(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
							 const FMargin& DefaultValue = FStyleDefaults::GetMargin()
								 MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	UMaterialInterface* GetNamedMaterial(const FName PropertyName, TOptional<FString> Specifier);
	UMaterialInterface* GetDefaultRoundedRectangleMaterial();
	UMaterialInterface* GetDefaultCheckboxMaterial();
	UMaterialInterface* GetGlyphMaterial(const FName PropertyName);
	const FSlateBrush* GetBrush(const FName PropertyName,
								const ANSICHAR* Specifier = nullptr MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	const TSharedPtr<FSlateBrush> GetGlyphBrush(const FName PropertyName, const FVector2D& Size);

	const TSharedPtr<FSlateBrush> GetMaterialBrush(const FName PropertyName, TOptional<FString> Specifier,
												   const FVector2D& Size);

	const TSharedPtr<FSlateBrush> GetMaterialBrushInternal(const FName PropertyName, UMaterialInterface* ParentMaterial,
														   const FVector2D& Size);

	const FSlateBrush* GetSlateColorBrush(const FName PropertyName, const ANSICHAR* Specifier = nullptr,
										  const FSlateBrush* const DefaultBrush = FStyleDefaults::GetNoBrush());

	const FSlateBrush* GetOptionalBrush(
		const FName PropertyName, const ANSICHAR* Specifier = nullptr,
		const FSlateBrush* const DefaultBrush = FStyleDefaults::GetNoBrush()) const override;

	const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(
		const FName BrushTemplate, const FName TextureName,
		const ANSICHAR* Specifier = nullptr MODIO_UE5_REQUESTING_STYLE_PARAM) MODIO_UE5_CONST override;

	const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(
		const FName BrushTemplate, const ANSICHAR* Specifier, UTexture2D* TextureResource,
		const FName TextureName MODIO_UE5_REQUESTING_STYLE_PARAM) MODIO_UE5_CONST override;

	const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(
		const FName BrushTemplate, UTexture2D* TextureResource,
		const FName TextureName MODIO_UE5_REQUESTING_STYLE_PARAM) MODIO_UE5_CONST override;

	virtual const TSharedPtr<FSlateDynamicImageBrush> MakeDynamicImageBrush(
		const FName BrushTemplate, UTexture2D* TextureResource, const FName TextureName) const MODIO_UE5_OVERRIDE;

	FSlateBrush* GetDefaultBrush() const override;

	const FSlateSound& GetSound(const FName PropertyName,
								const ANSICHAR* Specifier = nullptr MODIO_UE5_REQUESTING_STYLE_PARAM) const override;

	FSlateFontInfo GetFontStyle(const FName PropertyName, const ANSICHAR* Specifier = nullptr) const override;

	void NativeSerializeStyleReference(FString PathToProperty, FName StyleElementReference) override;

	TArray<FName> GetAllStyleNames() const;

	TArray<FName> GetNamedBrushNames();

	TArray<FName> GetMaterialNames();

	TArray<FName> GetGlyphNames();

	void ModifyRoundedRectangle(FName InName, UMaterialInstanceDynamic* MaterialInstance);

	class UModioColorPresets* GetColorPaletteData();

	UMaterialInterface* NativeGetBackgroundMaterial()
	{
		if (!DefaultBackgroundMaterial)
		{
			FSoftObjectPath Fallback("/modio/UI/Materials/M_UI_Background_Waves.M_UI_Background_Waves");
			DefaultBackgroundMaterial = Cast<UMaterialInterface>(Fallback.TryLoad());
		}
		return DefaultBackgroundMaterial;
	}
};
