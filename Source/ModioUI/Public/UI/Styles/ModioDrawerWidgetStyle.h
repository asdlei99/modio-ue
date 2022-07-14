#pragma once

#include "Styling/SlateTypes.h"
#include "UI/Styles/ModioUIColorRef.h"
#include "UI/Styles/ModioUIWidgetStyleContainer.h"

#include "ModioDrawerWidgetStyle.generated.h"

USTRUCT(BlueprintType)
struct MODIOUI_API FModioDrawerWidgetStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	static const FName TypeName;
	virtual const FName GetTypeName() const override
	{
		return TypeName;
	};

	static const FModioDrawerWidgetStyle& GetDefault()
	{
		static FModioDrawerWidgetStyle Default;
		return Default;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush HeadingBackgroundBrush;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush ContentBackgroundBrush;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush FooterBackgroundBrush;
};

UCLASS(meta = (DisplayName = "ModioDrawerWidgetStyle"))
class UModioDrawerWidgetStyleContainer : public UModioUIWidgetStyleContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ShowOnlyInnerProperties))
	FModioDrawerWidgetStyle Style;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return &Style;
	}
};