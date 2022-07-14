#pragma once

#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateNoResource.h"
#include "Styling/SlateTypes.h"
#include "UI/Styles/ModioUIStyleRef.h"
#include "UI/Styles/ModioWidgetBorderStyle.h"

#include "ModioCheckBoxStyle.generated.h"

USTRUCT(BlueprintType)
struct MODIOUI_API FModioCheckBoxStyle : public FSlateWidgetStyle
{
	GENERATED_BODY()

	static const FName TypeName;
	virtual const FName GetTypeName() const override
	{
		return TypeName;
	};

	static const FModioCheckBoxStyle& GetDefault()
	{
		static FModioCheckBoxStyle Default;
		return Default;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCheckBoxStyle CheckBoxStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FMargin ContentPadding;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTextBlockStyle TextStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (StyleClass = "ModioRichTextStyle"))
	FModioUIStyleRef TextStyleNew;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FModioWidgetBorderStyle CheckBoxBorderStyle;
};

UCLASS(meta = (DisplayName = "ModioCheckBoxStyle"))
class UModioCheckBoxStyleContainer : public UModioUIWidgetStyleContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ShowOnlyInnerProperties))
	FModioCheckBoxStyle Style;

	virtual void PostLoad() override
	{
		Super::PostLoad();
	}

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return &Style;
	}
};