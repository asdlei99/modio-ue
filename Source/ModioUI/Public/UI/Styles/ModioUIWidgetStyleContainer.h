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

#include "Core/ModioNewPropertyHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/EngineVersionComparison.h"
#include "ModioUI.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "PropertyPathHelpers.h"
#include "Styling/SlateWidgetStyleContainerBase.h"
#include "UI/Styles/ModioUIBrushRef.h"
#include "UI/Styles/ModioUIColorRef.h"

#if UE_VERSION_NEWER_THAN(5, 0, 0)
	#include "UObject/ObjectSaveContext.h"
#endif

#include "ModioUIWidgetStyleContainer.generated.h"

PRAGMA_DISABLE_OPTIMIZATION

/**
 * Strong type reference to a slate color
 **/
USTRUCT()
struct FModioSlateColorInspector : public FSlateColor
{
	GENERATED_BODY();
	/**
	 * Default constructor without parameters
	 **/
	FModioSlateColorInspector() : FSlateColor() {};

	/**
	 * Convenience constructor with a slate color parameter
	 * @param InColor The slate color to store for this instance
	 **/
	FModioSlateColorInspector(FSlateColor InColor) : FSlateColor(InColor) {};

	/**
	 * Ask the instance if the color is linked to use particular style
	 * @return True if the current color style was set to "UseColor_Specified", otherwise false
	 **/
	bool IsUnlinked()
	{
		return ColorUseRule == ESlateColorStylingMode::UseColor_Specified;
	}
};

UCLASS(BlueprintType, EditInlineNew, collapseCategories, MinimalAPI,
	   meta = (MaterialSerialize = "SerializedMaterials", ColorSerialize = "SerializedColors"))
class UModioUIWidgetStyleContainer : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere,
			  meta = (ReadOnlyKeys, EditCondition = "true==false", EditConditionHides, ShowOnlyInnerProperties,
					  EditFixedSize, EditFixedOrder),
			  Category = "Widgets")
	TMap<FName, FModioUIColorRef> SerializedColors;

	UPROPERTY(EditAnywhere,
			  meta = (ReadOnlyKeys, EditCondition = "true==false", EditConditionHides, ShowOnlyInnerProperties,
					  EditFixedSize, EditFixedOrder),
			  Category = "Widgets")
	TMap<FName, FModioUIMaterialRef> SerializedMaterials;

public:
#if UE_VERSION_NEWER_THAN(5, 0, 0)
	virtual void PreSave(FObjectPreSaveContext SaveContext) override
#else
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override
#endif
	{
		/* Causes colors to randomly lose value in packaged builds*/
		//for (FPropertyValueIterator It = FPropertyValueIterator(FStructProperty::StaticClass(), GetClass(), this); It;
		//	 ++It)
		//{
		//	const FStructProperty* Prop = CastField<FStructProperty>(It->Key);

		//	const void* Value = It->Value;
		//	if (Prop->Struct == FSlateColor::StaticStruct())
		//	{
		//		FSlateColor* ActualColor = const_cast<FSlateColor*>(reinterpret_cast<const FSlateColor*>(Value));
		//		if (!FModioSlateColorInspector(*ActualColor).IsUnlinked())
		//		{
		//			// UE_LOG(LogModioUI, Display, TEXT("Unlinking color %s in %s"), *Prop->GetAuthoredName(),
		//			//	   *GetFName().ToString());
		//			ActualColor->Unlink();
		//		}
		//	}
		//}

		for (const TPair<FName, FModioUIMaterialRef>& SerializedBrush : SerializedMaterials)
		{
			UMaterialInterface* BrushMaterial = SerializedBrush.Value.ResolveReference();

			if (BrushMaterial)
			{
				if (!ModioNewPropertyHelpers::SetPropertyValue<UObject*>(this, SerializedBrush.Key.ToString(),
																		 BrushMaterial))
				{
					ModioNewPropertyHelpers::SetPropertyValue<UObject*>(
						this, SerializedBrush.Key.ToString() + ".ResourceObject", BrushMaterial);
				}
			}
		}

#if UE_VERSION_NEWER_THAN(5, 0, 0)
		Super::PreSave(SaveContext);
#else
		Super::PreSave(TargetPlatform);
#endif
	};


	void LoadBrushMaterials()
	{
		/* Causes colors to randomly lose value in packaged builds*/
		/*for (FPropertyValueIterator It = FPropertyValueIterator(FStructProperty::StaticClass(), GetClass(), this); It;
			 ++It)
		{
			const FStructProperty* Prop = CastField<FStructProperty>(It->Key);

			const void* Value = It->Value;
			if (Prop->Struct == FSlateColor::StaticStruct())
			{
				FSlateColor* ActualColor = const_cast<FSlateColor*>(reinterpret_cast<const FSlateColor*>(Value));
				if (!FModioSlateColorInspector(*ActualColor).IsUnlinked())
				{
					 UE_LOG(LogModioUI, Display, TEXT("Unlinking color %s in %s"), *Prop->GetAuthoredName(),
						   *GetFName().ToString());
					ActualColor->Unlink();
				}
			}
		}*/
		//SerializedColors.Remove(NAME_None);

		//for (const TPair<FName, FModioUIColorRef>& SerializedColor : SerializedColors)
		//{
		//	TArray<FString> PropertyPathElems;
		//	if (!ModioNewPropertyHelpers::SetPropertyValue(this, SerializedColor.Key.ToString(),
		//												   SerializedColor.Value.ResolveReference()))
		//	{
		//		// UE_LOG(LogModioUI, Warning,
		//		//	   TEXT("Unable to restore serialized value for %s in %s"), *SerializedColor.Key.ToString(),
		//		//*GetFName().ToString());
		//	}
		//}

		for (const TPair<FName, FModioUIMaterialRef>& SerializedBrush : SerializedMaterials)
		{
			UMaterialInterface* BrushMaterial = SerializedBrush.Value.ResolveReference();
			
			if (BrushMaterial)
			{
				if (!ModioNewPropertyHelpers::SetPropertyValue<UObject*>(this, SerializedBrush.Key.ToString(),
																		 BrushMaterial))
				{
					ModioNewPropertyHelpers::SetPropertyValue<UObject*>(
						this, SerializedBrush.Key.ToString() + ".ResourceObject", BrushMaterial);
				}
			}
		}
	}
};

PRAGMA_ENABLE_OPTIMIZATION